/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "result_code.h"

typedef double Coordinate;

extern Coordinate kCoordinateMax;
extern Coordinate kCoordinateMin;

typedef struct Point {
  Coordinate x;
  Coordinate y;
} Point;

typedef struct PointDistance {
  Point p1;
  Point p2;
  Coordinate dist;
} PointDistance;

typedef ResultCode (*ClosestPairAlgo)(const size_t n, const Point points[n],
                                      PointDistance* result);

bool CoordinatesAreEqual(Coordinate c1, Coordinate c2);

/*
 * Calculates the Euclidean distance between two points
 *
 * params:
 *  <p1>     the first point
 *  <p2>     the second point
 *  <result> output parameter that will be set to result
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode EuclideanDistance(const Point* p1, const Point* p2,
                             Coordinate* result);

/*
 * Finds the closest points in the array
 *
 * params:
 *  <n>      length of input array (must be >= 2)
 *  <points> array of point objects
 *  <result> object to populate with calculation results
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode ClosestPair_Naive(const size_t n, const Point points[n],
                             PointDistance* result);

/*
 * Finds the closest points in the array
 *
 * params:
 *  <n>      length of input array (must be >= 2)
 *  <points> array of point objects
 *  <result> object to populate with calculation results
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode ClosestPair(const size_t n, const Point points[n],
                       PointDistance* result);
