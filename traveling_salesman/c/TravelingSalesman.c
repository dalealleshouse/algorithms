#include <math.h>
#include <stdlib.h>

#include "include/Math.h"

#include "TravelingSalesman.h"

ResultCode dist(const point* p1, const point* p2, double* result) {
  if (p1 == NULL || p2 == NULL || result == NULL) return NullParameter;

  double _dist =
      ((p2->x - p1->x) * (p2->x - p1->x)) + ((p2->y - p1->y) * (p2->y - p1->y));

  *result = sqrt(_dist);
  return Success;
}

ResultCode adjacencyMatrix(const city* cities, const size_t n,
                           double** matrix) {
  if (cities == NULL || matrix == NULL) return NullParameter;
  if (*matrix != NULL) return OutputPointerIsNotNull;

  double* result = malloc(sizeof(double) * n * n);
  if (result == NULL) return FailedMemoryAllocation;

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j)
      dist(&cities[i].coordinates, &cities[j].coordinates, &result[i * n + j]);
  }

  *matrix = result;
  return Success;
}

ResultCode TravelingSalesman(size_t n, double graph[n][n],
                             double* shortest_path) {
  if (graph == NULL || shortest_path == NULL) return NullParameter;
  if (n < 2) return ArgumentOutOfRange;

  unsigned int size = 1 << (n - 1);
  double* pBest = malloc(sizeof(double) * size * n);
  double(*best)[n] = (double(*)[n])pBest;

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < n; j++) {
      best[i][j] = INFINITY;
    }
  }

  for (size_t visited = 1; visited < (size); ++visited) {
    for (size_t last = 0; last < (n - 1); ++last) {
      // last visited vertex must be one of the visited vertices
      if (!(visited & 1 << last)) continue;

      // try all possibilities for the previous vertex,
      // pick the best among them
      if (visited == 1 << last) {
        // previous vertex must have been N-1
        best[visited][last] = graph[n - 1][last];
      } else {
        // previous vertex was one of the other ones in "visited"
        size_t prev_visited = visited ^ 1 << last;
        for (size_t prev = 0; prev < n - 1; ++prev) {
          if (!(prev_visited & 1 << prev)) continue;
          best[visited][last] =
              min_double(2, best[visited][last],
                         graph[last][prev] + best[prev_visited][prev]);
        }
      }
    }
  }

  double answer = INFINITY;
  for (size_t last = 0; last < n - 1; ++last)
    answer = min_double(2, answer, graph[last][n - 1] + best[(size - 1)][last]);

  free(best);
  *shortest_path = answer;

  return Success;
}
