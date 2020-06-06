#include "./graph.h"
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct EdgeTuple {
  int head;
  int weight;
} EdgeTuple;

const static size_t BUFFER_SIZE = 1024;

static void EdgeDestroy(Edge* edge) { free(edge); }

static void EdgesDestory(Edge* edge) {
  Edge* curr = edge;
  while (curr != NULL) {
    Edge* temp = curr->next;
    EdgeDestroy(curr);
    curr = temp;
  }
}

static void VertexDestroy(Vertex* vertex, freer freer) {
  if (vertex == NULL) return;

  if (freer != NULL) freer(vertex->data);

  EdgesDestory(vertex->edges);
  EdgesDestory(vertex->in_edges);

  free(vertex);
}

/*
 * This function is a bit over-engineered for a private function, but it was
 * orginally meant to be public. It was eaiser to just create all the vertices
 * during create than try to add them in after the fact and keep track of nulls
 * and such.
 */
static GraphResult Graph_AddVertex(Graph* self, vertex_id id, void* data) {
  if (self == NULL) return Graph_NullParameter;

  if (id < 0) return Graph_InvalidVertexId;

  if (id >= self->n) return Graph_VertexIdExceedsMaxSize;

  if (self->V[id] != NULL) return Graph_DuplicateVertexId;

  Vertex* v = calloc(sizeof(Vertex), 1);
  if (v == NULL) return Graph_FailedMemoryAllocation;

  v->id = id;
  v->data = data;

  self->V[id] = v;

  return Graph_Success;
}

static void AddVertex_PrintError(Graph* self, vertex_id id) {
  GraphResult result = Graph_AddVertex(self, id, NULL);
  if (result != Graph_Success) GRAPH_ERROR(result);  // NOLINT
}

Graph* Graph_Create(size_t n) {
  Graph* graph = malloc(sizeof(Graph));

  if (graph == NULL) {
    GRAPH_ERROR(Graph_FailedMemoryAllocation);  // NOLINT
    return NULL;
  }

  graph->V = calloc(sizeof(Vertex*), n);
  graph->m = 0;
  graph->n = n;

  for (size_t i = 0; i < n; i++) AddVertex_PrintError(graph, i);

  return graph;
}

static Edge* Graph_EdgeInit(vertex_id head, vertex_id tail, int weight) {
  Edge* edge = calloc(sizeof(Edge), 1);
  if (edge == NULL) return NULL;

  edge->head = head;
  edge->tail = tail;
  edge->weight = weight;

  return edge;
}

// It is assumed that all parameters are validated by the caller
static GraphResult Graph_AddInEdge(Graph* self, vertex_id head, vertex_id tail,
                                   int weight) {
  Edge* edge = Graph_EdgeInit(head, tail, weight);
  if (edge == NULL) return Graph_FailedMemoryAllocation;

  edge->next = self->V[head]->in_edges;
  self->V[head]->in_degree++;
  self->V[head]->in_edges = edge;

  return Graph_Success;
}

// It is assumed that all parameters are validated by the caller
static GraphResult Graph_AddOutEdge(Graph* self, vertex_id head, vertex_id tail,
                                    int weight) {
  Edge* edge = Graph_EdgeInit(head, tail, weight);
  if (edge == NULL) return Graph_FailedMemoryAllocation;

  edge->next = self->V[tail]->edges;
  self->V[tail]->out_degree++;
  self->V[tail]->edges = edge;

  return Graph_Success;
}

GraphResult Graph_AddWeightedEdge(Graph* self, vertex_id head, vertex_id tail,
                                  int weight) {
  if (self == NULL) return Graph_NullParameter;

  if (head >= self->n || tail >= self->n) return Graph_VertexIdExceedsMaxSize;

  if (self->V[head] == NULL || self->V[tail] == NULL)
    return Graph_InvalidVertexId;

  GraphResult result;

  result = Graph_AddInEdge(self, head, tail, weight);
  if (result != Graph_Success) return result;

  result = Graph_AddOutEdge(self, head, tail, weight);
  if (result != Graph_Success) return result;

  self->m++;

  return Graph_Success;
}

GraphResult Graph_AddEdge(Graph* self, vertex_id head, vertex_id tail) {
  return Graph_AddWeightedEdge(self, head, tail, 1);
}

static GraphResult non_weight_parser(Graph* g, FILE* file) {
  char line[BUFFER_SIZE];
  while (fgets(line, BUFFER_SIZE, file)) {
    char* remaining;
    vertex_id vertex = strtoumax(line, &remaining, 10);

    vertex_id edge = strtoul(remaining, &remaining, 10);
    while (edge != 0) {
      GraphResult result = Graph_AddEdge(g, edge, vertex);
      if (result != Graph_Success) return result;

      edge = strtoumax(remaining, &remaining, 10);
    }
  }

  return Graph_Success;
}

static EdgeTuple parse_edge_tuple(char* tuple) {
  char* remaining;
  vertex_id head = strtoumax(tuple, &remaining, 10);

  // ignore the comma
  remaining++;
  int weight = strtoimax(remaining, &remaining, 10);

  EdgeTuple t = {head, weight};
  return t;
}

static GraphResult weighted_parser(Graph* g, FILE* file) {
  char line[BUFFER_SIZE];
  while (fgets(line, BUFFER_SIZE, file)) {
    char* remaining;
    vertex_id vertex = strtoumax(line, &remaining, 10);

    const char* seperator = "\t";
    char* tok;

    tok = strtok(remaining, seperator);

    while (tok != NULL && strcmp(tok, "\n") != 0) {
      EdgeTuple t = parse_edge_tuple(tok);
      GraphResult result = Graph_AddWeightedEdge(g, t.head, vertex, t.weight);
      if (result != Graph_Success) return result;

      tok = strtok(0, seperator);
    }
  }

  return Graph_Success;
}

typedef GraphResult (*line_parser)(Graph*, FILE*);
static Graph* Graph_Parser(const size_t n, const char* path,
                           line_parser line_parser) {
  if (path == NULL) {
    GRAPH_ERROR(Graph_NullParameter);  // NOLINT
    return NULL;
  }

  if (access(path, R_OK) != 0) {
    GRAPH_ERROR(Graph_InvalidFilePath);  // NOLINT
    return NULL;
  }

  FILE* file = fopen(path, "r");
  if (file == NULL) {
    return NULL;
  }

  Graph* g = Graph_Create(n);
  if (g == NULL) {
    GRAPH_ERROR(Graph_FailedMemoryAllocation);  // NOLINT
    fclose(file);
    return NULL;
  }

  GraphResult result = line_parser(g, file);
  fclose(file);

  if (result != Graph_Success) {
    Graph_Destroy(g, NULL);
    GRAPH_ERROR(result);  // NOLINT
    return NULL;
  }

  return g;
}

Graph* Graph_FromFile(const size_t n, const char* path) {
  return Graph_Parser(n, path, non_weight_parser);
}

Graph* Graph_WeightedFromFile(const size_t n, const char* path) {
  return Graph_Parser(n, path, weighted_parser);
}

static Graph* _weightedFromFile(const char* path, bool undirected) {
  const static int BUFFER_SIZE = 1024;

  if (access(path, R_OK) != 0) {
    printf("File does not exist or access denied\n");
    return NULL;
  }

  FILE* file = fopen(path, "r");

  // Size is the first line of the file
  char line[BUFFER_SIZE];
  fgets(line, BUFFER_SIZE, file);

  size_t n = strtoul(line, NULL, 10);
  if (n == 0) {
    printf("Failed size_t conversion\n");
    return NULL;
  }

  Graph* graph = Graph_Create(n + 1);

  vertex_id node1, node2;
  int weight;
  char* remaining;
  while (fgets(line, BUFFER_SIZE, file)) {
    node1 = strtol(line, &remaining, 10);
    if (line == remaining) {
      printf("Failed int conversion\n");
      return graph;
    }

    char* temp = remaining;
    node2 = strtol(temp, &remaining, 10);
    if (temp == remaining) {
      printf("Failed int conversion\n");
      return graph;
    }

    errno = 0;
    weight = strtoimax(remaining, NULL, 10);
    if (errno != 0) {
      printf("Failed int conversion\n");
      printf("%s\n", strerror(errno));
      return graph;
    }

    if (undirected) Graph_AddWeightedEdge(graph, node1, node2, weight);
    Graph_AddWeightedEdge(graph, node2, node1, weight);
  }

  fclose(file);
  return graph;
}

Graph* Graph_WeightedFromFileUndirected(const char* path) {
  return _weightedFromFile(path, true);
}

Graph* Graph_WeightedFromFileDirected(const char* path) {
  return _weightedFromFile(path, false);
}

void Graph_Destroy(Graph* self, freer freer) {
  if (self == NULL) return;

  for (size_t i = 0; i < self->n; i++) VertexDestroy(self->V[i], freer);

  free(self->V);
  free(self);
}

char* Graph_ErrorMessage(GraphResult result) {
  switch (result) {
    case Graph_ArithmeticOverflow:
      return "An arithmetic overflow occured.";
    case Graph_NegativeCycle:
      return "A negative cycle was detected.";
    case Graph_DependencyError:
      return "One of the Graphs dependencies had an unrecoverable error.";
    case Graph_FileOpenError:
      return "fopen returned NULL";
    case Graph_InvalidFilePath:
      return "Invalid file path";
    case Graph_VertexIdExceedsMaxSize:
      return "The ID of the vertex is greater than max size";
    case Graph_DuplicateVertexId:
      return "Attempt to add two vertices with the same id";
    case Graph_InvalidVertexId:
      return "Invalid vertex ID";
    case Graph_FailedMemoryAllocation:
      return "Failed to allocate memory";
    case Graph_NullParameter:
      return "One of the required parameters passed to the function is NULL";
    case Graph_Success:
      return "Success";
    default:
      return "Unknown error code";
  }
}
