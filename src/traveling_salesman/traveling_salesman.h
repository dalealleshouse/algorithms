#pragma once

#include <stdbool.h>

#include "result_code.h"

typedef unsigned int id;

typedef struct point {
  double x;
  double y;
} point;

typedef struct city {
  id id;
  point coordinates;
  bool visted;
} city;

ResultCode dist(const point* p1, const point* p2, double* result);
ResultCode adjacencyMatrix(const city* cities, const size_t n, double** matrix);
ResultCode TravelingSalesman(size_t n, double graph[n][n],
                             double* shortest_path);
ResultCode TravelingSalesmanApprox(size_t n, city* cites,
                                   double* shortest_path);
