/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include <stdlib.h>
#include <time.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

const char* kSuiteName = "CUNIT_SUITE";

// utils
extern int RegisterGraphTests(void);
extern int RegisterMathTests(void);
// Common
extern int RegisterCommonTests(void);

// graph search
extern int RegisterGraphSearchTests(void);

// Data Structures
extern int RegisterQueueTests(void);
extern int RegisterStackTests(void);
extern int RegisterHeapTests(void);
extern int RegisterHashTableTests(void);
extern int RegisterBloomFilterTests(void);
extern int RegisterHashFunctionTests(void);
extern int RegisterDisjointSetTests(void);
extern int RegisterDisjointSetTestCase(void);

// Knapsack Problem
extern int RegisterKnapsackProblemTests(void);

// Running Median
extern int RegisterRunningMedianTests(void);
extern int RegisterRunningMedianTestCase(void);

// Sequence Alignment
extern int RegisterSequenceAlignmentTests(void);

// Shortest Path
extern int RegisterDijkstraTests(void);
extern int RegisterShortestPathTests(void);
extern int RegisterBellmanFordTests(void);
extern int RegisterFloydWarshallTests(void);

// Huffman Codes
extern int RegisterHuffmanCodeTests(void);

// List Data Structures
extern int RegisterLinkedListTests(void);
extern int RegisterArrayTests(void);
extern int RegisterAlgoTimerTests(void);
extern int RegisterSortedArrayTests(void);
extern int RegisterBinaryTreeTests(void);

// Min Spanning Tree
extern int RegisterMinSpanningTreeTests(void);
extern int RegisterKruskalClusterTests(void);

// Random Contraction
extern int RegisterGraphTests(void);
extern int RegisterMinCutTests(void);

// Satisfiablity
extern int RegisterSatisfiabilityTests(void);

// Scheudule Optimizer
int RegisterScheduleOptimizerTests(void);

// Sorting
extern int RegisterQuickSortTests(void);
extern int RegisterExtraQuickSortTests(void);
extern int RegisterPartitionTests(void);
extern int RegisterBubbleSortTests(void);
extern int RegisterBubbleSortTestCase(void);
extern int RegisterInsertionSortTests(void);
extern int RegisterInsertionSortTestCase(void);
extern int RegisterSelectionSortTests(void);
extern int RegisterMergeSortAdapterTests(void);
extern int RegisterSortInstrumentationTestCase(void);

// Traveling Salesman
extern int RegisterTravelingSalesmanTests(void);

// Weighted Independent Sets
extern int RegisterWeightedIndependentSetTests(void);

// Malloc Test Wrapper
extern int RegisterMallocTestWrapperTests(void);

// Cloest Distance
extern int RegisterClosestDistanceTests(void);
extern int RegisterClosestPairTestCase(void);

// Quick Select
extern int RegisterSelectTests(void);
extern int RegisterQuickSelectTests(void);

// Inversion Couht
extern int RegisterInversionCountTests(void);

// Matrix Multiplication
extern int RegisterMatrixOperationsTests(void);
extern int RegisterMatrixOperationTestCase(void);

// Cache
extern int RegisterCacheTests(void);

extern int RegisterBitVectorTests(void);

int RegisterTests(void) {
  // Seed the random number generator
  struct timespec ts;
  if (timespec_get(&ts, TIME_UTC) == 0) {
    int result = fprintf(stderr, "timespec_get failed");
    if (result < 0) perror("fprintf failed");
    return -1;
  }
  srandom(ts.tv_nsec ^ ts.tv_sec);
  srand48(ts.tv_nsec ^ ts.tv_sec);

  return (
  // clang-format off
#if defined(TEST_CASES)
      // Test Cases
      RegisterDisjointSetTestCase(void) != 0 +
      RegisterBubbleSortTestCase(void) != 0 +
      RegisterInsertionSortTestCase(void) != 0 +
      RegisterSortInstrumentationTestCase(void) != 0 +
      RegisterMatrixOperationTestCase(void) != 0 +
      RegisterRunningMedianTestCase(void) != 0 +
      RegisterClosestPairTestCase(void) != 0 +
#else
      // Unit tests
      RegisterGraphTests() != 0 +
      RegisterMathTests() != 0 +
      RegisterCommonTests() != 0 +
      RegisterGraphSearchTests() != 0 +
      RegisterQueueTests() != 0 +
      RegisterStackTests() != 0 +
      RegisterHeapTests() != 0 +
      RegisterHashTableTests() != 0 +
      RegisterBloomFilterTests() != 0 +
      RegisterHashFunctionTests() != 0 +
      RegisterDisjointSetTests() != 0 +
      RegisterKnapsackProblemTests() != 0 +
      RegisterRunningMedianTests() != 0 +
      RegisterSequenceAlignmentTests() != 0 +
      RegisterDijkstraTests() != 0 +
      RegisterShortestPathTests() != 0 +
      RegisterBellmanFordTests() != 0 +
      RegisterFloydWarshallTests() != 0 +
      RegisterHuffmanCodeTests() != 0 +
      RegisterLinkedListTests() != 0 +
      RegisterArrayTests() != 0 +
      RegisterSortedArrayTests() != 0 +
      RegisterBinaryTreeTests() != 0 +
      RegisterGraphTests() != 0 +
      RegisterMinCutTests() != 0 +
      RegisterSatisfiabilityTests() != 0 +
      RegisterQuickSortTests() != 0 +
      RegisterExtraQuickSortTests() != 0 +
      RegisterPartitionTests() != 0 +
      RegisterBubbleSortTests() != 0 +
      RegisterInsertionSortTests() != 0 +
      RegisterSelectionSortTests() != 0 +
      RegisterMergeSortAdapterTests() != 0 +
      RegisterTravelingSalesmanTests() != 0 +
      RegisterWeightedIndependentSetTests() != 0 +
      RegisterMallocTestWrapperTests() != 0 +
      RegisterMinSpanningTreeTests() != 0 +
      RegisterKruskalClusterTests() != 0 +
      RegisterClosestDistanceTests() != 0 +
      RegisterSelectTests() != 0 +
      RegisterQuickSelectTests() != 0 +
      RegisterInversionCountTests() != 0 +
      RegisterMatrixOperationsTests() != 0 +
      RegisterCacheTests() != 0 +
      RegisterBitVectorTests() != 0 +
#endif
      0) * -1;
  // clang-format on
}

static CU_pSuite FindSuite(char* suite_name) {
  CU_pTestRegistry registry = CU_get_registry();
  CU_pSuite suite = registry->pSuite;

  while (suite != NULL) {
    if (strcmp(suite_name, suite->pName) == 0) return suite;
    suite = suite->pNext;
  }

  int result = fprintf(stderr, "Suite not found: %s\n", suite_name);
  if (result < 0) perror("fprintf failed");
  return NULL;
}

int TestRunner(int (*register_tests)(void)) {
  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

  if (register_tests() != 0) {
    CU_cleanup_registry();
    return -1;
  }

  // Run all tests using the basic interface
  CU_basic_set_mode(CU_BRM_VERBOSE);

  // If the env variable is set, run just that suite, otherwise run them all
  char* suite_name = getenv(kSuiteName);
  if (suite_name == NULL) {
    CU_basic_run_tests();
  } else {
    CU_pSuite suite = FindSuite(suite_name);
    if (suite) CU_basic_run_suite(suite);
  }

  // Get this value before CU_cleanup_registry(void) or it will revert to zero
  int ret = CU_get_number_of_failures() != 0;

  // Clean up registry and return
  CU_cleanup_registry();
  return ret;
}

int main(void) { return TestRunner(RegisterTests); }
