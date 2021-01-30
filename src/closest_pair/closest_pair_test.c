/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "closest_pair.h"

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "test_helpers.h"

const Coordinate kTolerance = 0.000001;

static bool PointsAreEqual(const Point* x, const Point* y) {
  return memcmp(x, y, sizeof(Point)) == 0;
}

static void ClosestPair_tester(ClosestPairAlgo algo, const size_t n,
                               const Point points[n], Coordinate expected,
                               size_t expected_p1, size_t expected_p2) {
  PointDistance answer = {.dist = 0};

  ResultCode result = algo(n, points, &answer);
  CU_ASSERT_EQUAL(kSuccess, result);
  CU_ASSERT_DOUBLE_EQUAL(answer.dist, expected, kTolerance);

  // The order of the points is irrelevant
  CU_ASSERT(PointsAreEqual(&points[expected_p1], &answer.p1) ||
            PointsAreEqual(&points[expected_p1], &answer.p2));
  CU_ASSERT(PointsAreEqual(&points[expected_p2], &answer.p1) ||
            PointsAreEqual(&points[expected_p2], &answer.p2));
}

static void ClosestPair_test_driver(ClosestPairAlgo algo) {
  const Coordinate expected = 1.414214;

  const Point points[] = {{2, 3}, {12, 30}, {40, 50}, {5, 1}, {12, 10}, {3, 4}};
  const int n = sizeof(points) / sizeof(points[0]);
  ClosestPair_tester(algo, n, points, expected, 0, 5);

  const Point points2[] = {{0, 0}, {-2, 0}, {4, 0}, {1, 1},
                           {3, 3}, {-2, 2}, {5, 2}};
  const int n2 = sizeof(points) / sizeof(points[0]);
  ClosestPair_tester(algo, n2, points2, expected, 0, 3);
}

Coordinate drand(Coordinate low, Coordinate high) {
  return ((Coordinate)random() * (high - low)) / (Coordinate)RAND_MAX + low;
}

/************* EuclideanDistance **********************************************/

static void EuclideanDistance_null_parameter() {
  Coordinate result;
  Point dummy = {1.5, -1.5};

  ResultCode result_code = EuclideanDistance(NULL, NULL, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = EuclideanDistance(&dummy, NULL, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = EuclideanDistance(NULL, &dummy, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = EuclideanDistance(NULL, NULL, &result);
  CU_ASSERT_EQUAL(kNullParameter, result_code);
}

static void EuclideanDistance_distance() {
  Coordinate dist;
  Point p1 = {1.5, -1.5};
  Point p2 = {7.5, -7.5};

  ResultCode result = EuclideanDistance(&p1, &p2, &dist);
  CU_ASSERT_EQUAL(kSuccess, result);
  CU_ASSERT_DOUBLE_EQUAL(dist, 8.485281, kTolerance);
}

static void EuclideanDistance_zero_distance() {
  Coordinate dist;
  Point p1 = {1.5, -1.5};
  Point p2 = {1.5, -1.5};

  ResultCode result = EuclideanDistance(&p1, &p2, &dist);
  CU_ASSERT_EQUAL(kSuccess, result);
  CU_ASSERT_DOUBLE_EQUAL(dist, 0, kTolerance);
}

static void EuclideanDistance_overflow() {
  Coordinate dist;
  Point p1 = {kCorrdinateMax, kCorrdinateMax};
  Point p2 = {kCorrdinateMin, kCorrdinateMin};

  ResultCode result = EuclideanDistance(&p1, &p2, &dist);
  CU_ASSERT_EQUAL(kArithmeticOverflow, result);
  CU_ASSERT(isinf(dist))
}

/************* ClosestPair_Naive **********************************************/
static void ClosestPair_Naive_null_parameter() {
  ResultCode result = ClosestPair_Naive(1, NULL, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result);
}

static void ClosestPair_Naive_invalid_size() {
  const Point points[] = {{kCorrdinateMax, 3}, {12, 30}};
  PointDistance answer = {.dist = 0};

  ResultCode result_code = ClosestPair_Naive(1, points, &answer);
  CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
}

static void ClosestPair_Naive_overflow() {
  const Point points[] = {{kCorrdinateMax, 3}, {12, 30}};
  const int n = sizeof(points) / sizeof(points[0]);
  PointDistance answer = {.dist = 0};

  ResultCode result_code = ClosestPair_Naive(n, points, &answer);
  CU_ASSERT_EQUAL(kArithmeticOverflow, result_code);
}

static void ClosestPair_Naive_happy_path() {
  ClosestPair_test_driver(ClosestPair_Naive);
}

/************* ClosestPair ****************************************************/
static void ClosestPair_null_parameter() {
  ResultCode result = ClosestPair(1, NULL, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result);
}

static void ClosestPair_invalid_size() {
  const Point points[] = {{kCorrdinateMax, 3}, {12, 30}};
  PointDistance answer = {.dist = 0};

  ResultCode result_code = ClosestPair(1, points, &answer);
  CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
}

static void ClosestPair_overflow() {
  const Point points[] = {{kCorrdinateMax, 3}, {12, 30}};
  const int n = sizeof(points) / sizeof(points[0]);
  PointDistance answer = {.dist = 0};

  ResultCode result_code = ClosestPair(n, points, &answer);
  CU_ASSERT_EQUAL(kArithmeticOverflow, result_code);
}

static void ClosestPair_happy_path() { ClosestPair_test_driver(ClosestPair); }

static void ClosestPair_matches_naive() {
  const size_t kN = 10000;
  Point points[kN];

  for (size_t i = 0; i < kN; i++) {
    points[i].x = drand(-50, 50);
    points[i].y = drand(-50, 50);
  }

  PointDistance slow = {.dist = 0};
  PointDistance fast = {.dist = 0};

  ResultCode result = ClosestPair_Naive(kN, points, &slow);
  CU_ASSERT_EQUAL(kSuccess, result);

  result = ClosestPair(kN, points, &fast);
  CU_ASSERT_EQUAL(kSuccess, result);

  CU_ASSERT_DOUBLE_EQUAL(slow.dist, fast.dist, kTolerance);

  CU_ASSERT(PointsAreEqual(&slow.p1, &fast.p1) ||
            PointsAreEqual(&slow.p1, &fast.p2));

  CU_ASSERT(PointsAreEqual(&slow.p2, &fast.p1) ||
            PointsAreEqual(&slow.p2, &fast.p2));
}

int RegisterClosestDistanceTests() {
  CU_TestInfo ClosestPair_tests[] = {
      CU_TEST_INFO(EuclideanDistance_null_parameter),
      CU_TEST_INFO(EuclideanDistance_zero_distance),
      CU_TEST_INFO(EuclideanDistance_overflow),
      CU_TEST_INFO(EuclideanDistance_distance),
      CU_TEST_INFO(ClosestPair_Naive_null_parameter),
      CU_TEST_INFO(ClosestPair_Naive_happy_path),
      CU_TEST_INFO(ClosestPair_Naive_invalid_size),
      CU_TEST_INFO(ClosestPair_Naive_overflow),
      CU_TEST_INFO(ClosestPair_null_parameter),
      CU_TEST_INFO(ClosestPair_invalid_size),
      CU_TEST_INFO(ClosestPair_overflow),
      CU_TEST_INFO(ClosestPair_happy_path),
      CU_TEST_INFO(ClosestPair_matches_naive),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "closest distance suite",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = ClosestPair_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
