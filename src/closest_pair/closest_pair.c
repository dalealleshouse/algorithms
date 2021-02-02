/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "closest_pair.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Coordinate kCorrdinateMax = DBL_MAX;
Coordinate kCorrdinateMin = DBL_MIN;

typedef Coordinate (*Getter)(Point* point);
static Coordinate XGetter(Point* point) { return point->x; }
static Coordinate YGetter(Point* point) { return point->y; }

static int PointComparator(const void* x, const void* y, Getter getter) {
  if (x == y) return 0;
  if (x == NULL) return -1;
  if (y == NULL) return 1;

  Point* p1 = (Point*)x;
  Point* p2 = (Point*)y;

  Coordinate test_result = getter(p1) - getter(p2);
  if (test_result > 0) return 1;
  if (test_result < 0) return -1;
  return 0;
}

static int PointXComparator(const void* x, const void* y) {
  return PointComparator(x, y, XGetter);
}

static int PointYComparator(const void* x, const void* y) {
  return PointComparator(x, y, YGetter);
}

static ResultCode ClosestPair_Recursive(const size_t n, const Point by_x[n],
                                        const Point by_y[n],
                                        PointDistance* result) {
  ResultCode result_code;

  // Base case
  if (n <= 3) return ClosestPair_Naive(n, by_x, result);

  // Split the array up into right and left halves
  size_t left_half = n >> 1;
  size_t right_half = n - left_half;

  // left and right halves sorted by x
  const Point* left_x = by_x;
  const Point* right_x = &by_x[left_half];

  // We need left and right halves sorted by y. These halves need to contain
  // the same points as the sorted by x halves. It would be possible to
  // simply sort them again using qsort, but that would raise the
  // computational complexity. In order to achieve O(n log(n)) this step must
  // be in linear time - O(n). We use the midpoint of the x value to split
  // the y sorted array into two parts.
  Coordinate mid_point = by_x[left_half].x;
  Point left_y[left_half];
  Point right_y[right_half];

  size_t left = 0, right = 0;
  for (size_t i = 0; i < n; ++i) {
    if (by_y[i].x < mid_point) {
      left_y[left++] = by_y[i];
    } else {
      right_y[right++] = by_y[i];
    }
  }

  PointDistance left_closest = {.dist = 0};
  PointDistance right_closest = {.dist = 0};

  result_code = ClosestPair_Recursive(left_half, left_x, left_y, &left_closest);
  if (result_code != kSuccess) return result_code;

  result_code =
      ClosestPair_Recursive(right_half, right_x, right_y, &right_closest);
  if (result_code != kSuccess) return result_code;

  PointDistance min =
      (left_closest.dist < right_closest.dist) ? left_closest : right_closest;

  // Find points closest to the vertical line
  Point strip[n];
  size_t size = 0;

  for (size_t i = 0; i < n; ++i) {
    if (fabs(by_y[i].x - mid_point) < min.dist) {
      strip[size] = by_y[i];
      size++;
    }
  }

  for (size_t i = 0; i < size; ++i) {
    // As strange as it sounds, this will never run more than 6 times
    for (size_t j = i + 1; j < size && (strip[j].y - strip[i].y) < min.dist;
         ++j) {
      Coordinate temp;
      result_code = EuclideanDistance(&strip[i], &strip[j], &temp);
      if (result_code != kSuccess) return result_code;

      if (temp < min.dist) {
        min.dist = temp;
        min.p1 = strip[j];
        min.p2 = strip[i];
      }
    }
  }
  *result = min;
  return kSuccess;
}

ResultCode EuclideanDistance(const Point* p1, const Point* p2,
                             Coordinate* result) {
  if (p1 == NULL || p2 == NULL || result == NULL) return kNullParameter;

  *result = sqrt(pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2));

  return (!isnormal(*result) && *result != 0) ? kArithmeticOverflow : kSuccess;
}

ResultCode ClosestPair_Naive(const size_t n, const Point points[n],
                             PointDistance* result) {
  if (points == NULL || result == NULL) return kNullParameter;
  if (n <= 1) return kArgumentOutOfRange;

  result->dist = kCorrdinateMax;

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = i + 1; j < n; ++j) {
      Coordinate dist;
      ResultCode result_code = EuclideanDistance(&points[i], &points[j], &dist);
      if (result_code != kSuccess) return result_code;

      if (dist < result->dist) {
        result->p1 = points[i];
        result->p2 = points[j];
        result->dist = dist;
      }
    }
  }

  return kSuccess;
}

ResultCode ClosestPair(const size_t n, const Point points[n],
                       PointDistance* result) {
  if (points == NULL || result == NULL) return kNullParameter;
  if (n <= 1) return kArgumentOutOfRange;

  size_t p_size = sizeof(points[0]);
  size_t arr_size = p_size * n;

  // Create two copies of the points
  Point by_x[n];
  Point by_y[n];
  memcpy(by_x, points, arr_size);
  memcpy(by_y, points, arr_size);

  // Sort the points
  qsort(by_x, n, p_size, PointXComparator);
  qsort(by_y, n, p_size, PointYComparator);

  return ClosestPair_Recursive(n, by_x, by_y, result);
}
