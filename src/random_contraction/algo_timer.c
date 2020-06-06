#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./graph.h"
#include "./random_contraction.h"

typedef enum { KARGER = 1, KARGER_STEIN = 2 } algo;
typedef Deprecated_Graph* (*min_cut)(const Deprecated_Graph*);

static min_cut get_selector(const algo algo) {
  switch (algo) {
    case KARGER:
      return RC_KargerMinCut;
    case KARGER_STEIN:
      return RC_KargerSteinMinCut;
    default:
      fprintf(stderr, "Invalid algorithm\n");
      return NULL;
  }
}

static unsigned RandomNumberBetween(unsigned low, unsigned high) {
  return (rand() % (high - low)) + low;
}

double AlgoTime(const size_t n, const size_t m, const algo algo) {
  min_cut min_cut = get_selector(algo);
  if (min_cut == NULL) return -1;

  Deprecated_Graph* graph = Deprecated_Graph_Create();
  if (graph == NULL) return -1;

  for (size_t i = 0; i < n; i++) {
    GraphResult result = Graph_AddVertex(graph, i + 1);

    if (result != Graph_Success) printf("vertex fail %d\n", result);
  }

  for (size_t i = 0; i < m; i++) {
    unsigned y = RandomNumberBetween(1, n);
    unsigned x = RandomNumberBetween(1, n);

    while (y == x) y = RandomNumberBetween(1, n);

    GraphResult result = Deprecated_Graph_AddEdge(graph, x, y);

    if (result != Graph_Success) {
      printf("edge fail %d, %d, %d \n", result, x, y);
    }
  }

  clock_t t = clock();
  Deprecated_Graph* result = min_cut(graph);
  t = clock() - t;

  if (result == NULL) return -1;

  Deprecated_Graph_Destroy(graph);

  double time = ((double)t) / CLOCKS_PER_SEC;
  return time;
}
