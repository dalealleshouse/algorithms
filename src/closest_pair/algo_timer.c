/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "algo_timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "closest_pair.h"

static ClosestPairAlgo GetAlgorithm(const ClosestPairAlgoirthm algo) {
  switch (algo) {
    case kNaiveClosestPair:
      return ClosestPair_Naive;
    case kClosestPair:
      return ClosestPair;
    default:
      return NULL;
  }
}

static Coordinate drand(Coordinate low, Coordinate high) {
  return ((Coordinate)random() * (high - low)) / (Coordinate)RAND_MAX + low;
}

static void GeneratePoints(size_t n, Point* points[n]) {
  *points = calloc(sizeof(Point), n);
  for (size_t i = 0; i < n; i++) {
    (*points)[i].x = drand(-50, 50);
    (*points)[i].y = drand(-50, 50);
  }
}

double ClosestPairTime(const size_t n, const ClosestPairAlgoirthm algo_type) {
  ResultCode result_code;
  double time = -1;

  // get a function pointer to the desired algorithm
  ClosestPairAlgo algo = GetAlgorithm(algo_type);
  if (algo == NULL) {
    fprintf(stderr, "Invalid algorithm: %d\n", algo_type);
    return -1;
  }

  Point* points[n];
  GeneratePoints(n, points);

  // start the timer
  clock_t t = clock();

  // run the algorithm
  PointDistance throw_away = {.dist = 0};
  result_code = algo(n, *points, &throw_away);

  // stop the timer
  t = clock() - t;

  if (result_code != kSuccess) {
    fprintf(stderr, "Algorithm returned an error code: %s",
            Result_ErrorMessage(result_code));
    goto done;
  }

  time = ((double)t) / CLOCKS_PER_SEC;

done:
  free(*points);
  return time;
}
