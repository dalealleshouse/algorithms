/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>

typedef struct point {
  double x;
  double y;
} point_t;

typedef struct point_dist {
  point_t p1;
  point_t p2;
  double dist;
} point_dist_t;

/*
 * Calculates the Euclidean distance between two points
 *
 * returns: 0 on kkSuccess and a negative number on failure
 */
int euclid_dist(const point_t* p1, const point_t* p2, double* result);

/*
 * Finds the closest points in the array
 *
 * params:
 *  - n = length of input array
 *  - points = array of point objects
 *  - result = object to populate with calculation results
 *
 * returns: 0 on kkSuccess, negative value on failure
 */
int closest_distance(const size_t n, const point_t points[n],
                     point_dist_t* result);

int closest_slow(const size_t n, const point_t points[n], point_dist_t* result);

void Closest_slow(const size_t n, const point_t points[n],
                  point_dist_t* result);
