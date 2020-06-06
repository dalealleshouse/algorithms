#include "./traveling_salesman.h"

#include <math.h>
#include <stdlib.h>

#include "../utils/math.h"

static double squared_dist(const point* p1, const point* p2) {
  return ((p2->x - p1->x) * (p2->x - p1->x)) +
         ((p2->y - p1->y) * (p2->y - p1->y));
}

static double _dist(const point* p1, const point* p2) {
  return sqrt(squared_dist(p1, p2));
}

ResultCode dist(const point* p1, const point* p2, double* result) {
  if (p1 == NULL || p2 == NULL || result == NULL) return NullParameter;

  *result = _dist(p1, p2);
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
              MinDouble(2, best[visited][last],
                        graph[last][prev] + best[prev_visited][prev]);
        }
      }
    }
  }

  double answer = INFINITY;
  for (size_t last = 0; last < n - 1; ++last)
    answer = MinDouble(2, answer, graph[last][n - 1] + best[(size - 1)][last]);

  free(best);
  *shortest_path = answer;

  return Success;
}

static city* find_closest(city* cities, size_t n, city* the_city) {
  city* result = NULL;
  double shortest_dist = INFINITY;

  for (size_t i = 0; i < n; i++) {
    if (cities[i].visted) continue;

    double dist = squared_dist(&the_city->coordinates, &cities[i].coordinates);

    if (dist < shortest_dist) {
      result = &cities[i];
      shortest_dist = dist;
    }
  }

  return result;
}

ResultCode TravelingSalesmanApprox(size_t n, city* cities,
                                   double* shortest_path) {
  if (cities == NULL || shortest_path == NULL) return NullParameter;
  if (n < 2) return ArgumentOutOfRange;

  city* here_now = &cities[0];
  here_now->visted = true;
  double dist_so_far = 0;

  for (size_t i = 1; i < n; i++) {
    city* closest = find_closest(cities, n, here_now);

    dist_so_far += _dist(&closest->coordinates, &here_now->coordinates);

    closest->visted = true;
    here_now = closest;
  }

  dist_so_far += _dist(&cities[0].coordinates, &here_now->coordinates);
  *shortest_path = dist_so_far;

  return Success;
}
