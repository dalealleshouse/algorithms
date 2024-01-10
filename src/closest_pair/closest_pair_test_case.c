/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "CUnit/CUnit.h"
#include "closest_pair.h"
#include "test_helpers.h"

static const size_t kSize = 1000000;
static Coordinate kEpsilion = 0.000001;
static const size_t kBufferSize = 128;
static const Coordinate kMaxCorrdinate = 50000;
static const char* kFile = "./src/closest_pair/test_data/points.txt";

static Coordinate drand(Coordinate low, Coordinate high) {
  return ((Coordinate)random() * (high - low)) / RAND_MAX + low;
}

static void GeneratePointsInputFile(const char* file_name) {
  FILE* points_file;

  points_file = fopen(file_name, "w+");
  if (points_file == NULL) {
    perror("Error opening file");
    return;
  }

  for (size_t i = 0; i < kSize; i++) {
    Coordinate x = drand(kMaxCorrdinate * -1, kMaxCorrdinate);
    Coordinate y = drand(kMaxCorrdinate * -1, kMaxCorrdinate);
    int result = fprintf(points_file, "%f, %f\n", x, y);
    if (result < 0) {
      int fprint_result = fprintf(stderr, "fprintf error %d\n", result);
      if (fprint_result < 0) perror("fprintf error");

      goto done;
    }
  }

done:
  fclose(points_file);
}

static int ReadPointsFromFile(const char* path, size_t n, Point (**points)[n]) {
  if (access(path, R_OK) != 0) {
    int fprint_result =
        fprintf(stderr, "File does not exist or access denied\n");
    if (fprint_result < 0) perror("fprintf error");

    return -1;
  }

  Point(*p)[n] = malloc(sizeof(*p));
  if (p == NULL) {
    int fprint_result = fprintf(stderr, "malloc failed\n");
    if (fprint_result < 0) perror("fprintf error");

    return -1;
  }

  FILE* points_file = fopen(path, "r");
  if (points_file == NULL) {
    free(p);
    perror("Error opening file");
    return -1;
  }

  char line[kBufferSize];
  size_t row = 0;
  while (fgets(line, (int)sizeof(line), points_file) && row < kSize) {
    char* endptr = NULL;
    Coordinate x = strtod(line, &endptr);
    (*p)[row].x = x;

    Coordinate y = strtod(endptr + 2, NULL);
    (*p)[row].y = y;

    ++row;
  }

  *points = p;
  int fclose_result = fclose(points_file);
  if (fclose_result == EOF) perror("fclose error");
  return 0;
}

static void FindClosestPairInFile(void) {
  (void)GeneratePointsInputFile;
  /* GeneratePointsInputFile(kFile); */

  Point(*points)[kSize] = NULL;
  if (ReadPointsFromFile(kFile, kSize, &points) != 0) goto fail;

  PointDistance result;
  ResultCode result_code = ClosestPair(kSize, *points, &result);
  if (result_code != kSuccess) goto result_code_fail;

  CU_ASSERT_DOUBLE_EQUAL(38524.043694, result.p1.x, kEpsilion);
  CU_ASSERT_DOUBLE_EQUAL(-33081.291468, result.p1.y, kEpsilion);

  CU_ASSERT_DOUBLE_EQUAL(38523.977990, result.p2.x, kEpsilion);
  CU_ASSERT_DOUBLE_EQUAL(-33081.309257, result.p2.y, kEpsilion);

  CU_ASSERT_DOUBLE_EQUAL(0.068070, result.dist, kEpsilion);

  printf("\nPoint 1=(%lf, %lf), Point 2=(%lf, %lf), Distance=%lf\n",
         result.p1.x, result.p1.y, result.p2.x, result.p2.y, result.dist);

  free(points);
  return;

result_code_fail:
  PRINT_ERROR("MultiplyLargeMatracies", result_code);
fail:
  free(points);
  CU_FAIL();
}

int RegisterClosestPairTestCase(void) {
  CU_TestInfo DisjointSetTests[] = {CU_TEST_INFO(FindClosestPairInFile),
                                    CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Closest Pair Test Case",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = DisjointSetTests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
