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
extern int RegisterGraphTests();
extern int RegisterMathTests();
// Common
extern int RegisterCommonTests();

// graph search
extern int RegisterGraphSearchTests();

// Data Structures
extern int RegisterQueueTests();
extern int RegisterStackTests();
extern int RegisterHeapTests();
extern int RegisterHashTableTests();
extern int RegisterBloomFilterTests();
extern int RegisterHashFunctionTests();
extern int RegisterDisjointSetTests();
extern int RegisterDisjointSetTestCase();

// Knapsack Problem
extern int RegisterKnapsackProblemTests();

// Running Median
extern int RegisterRunningMedianTests();

// Sequence Alignment
extern int RegisterSequenceAlignmentTests();

// Shortest Path
extern int RegisterDijkstraTests();
extern int RegisterShortestPathTests();
extern int RegisterBellmanFordTests();
extern int RegisterFloydWarshallTests();

// Huffman Codes
extern int RegisterHuffmanCodeTests();

// List Data Structures
extern int RegisterLinkedListTests();
extern int RegisterArrayTests();
extern int RegisterAlgoTimerTests();
extern int RegisterSortedArrayTests();
extern int RegisterBinaryTreeTests();

// Min Spanning Tree
extern int RegisterMinSpanningTreeTests();
extern int RegisterKruskalClusterTests();

// Random Contraction
extern int RegisterGraphTests();
extern int RegisterMinCutTests();

// Satisfiablity
extern int RegisterSatisfiabilityTests();

// Scheudule Optimizer
int RegisterScheduleOptimizerTests();

// Sorting
extern int RegisterQuickSortTests();
extern int RegisterExtraQuickSortTests();
extern int RegisterPartitionTests();
extern int RegisterBubbleSortTests();
extern int RegisterBubbleSortTestCase();
extern int RegisterInsertionSortTests();
extern int RegisterInsertionSortTestCase();
extern int RegisterSelectionSortTests();
extern int RegisterMergeSortAdapterTests();
extern int RegisterSortInstrumentationTestCase();

// Traveling Salesman
extern int RegisterTravelingSalesmanTests();

// Weighted Independent Sets
extern int RegisterWeightedIndependentSetTests();

// Malloc Test Wrapper
extern int RegisterMallocTestWrapperTests();

// Cloest Distance
extern int RegisterEuclidDistTests();
extern int RegisterClosestDistanceTests();
extern int RegisterClosestDistanceSlowTests();

// Quick Select
extern int RegisterSelectTests();
extern int RegisterQuickSelectTests();

// Inversion Couht
extern int RegisterInversionCountTests();

// Matrix Multiplication
extern int RegisterMatrixOperationsTests();
extern int RegisterMatrixOperationTestCase();

// Cache
extern int RegisterCacheTests();

extern int RegisterBitVectorTests();

int RegisterTests() {
  // Seed the random number generator
  struct timespec ts;
  if (timespec_get(&ts, TIME_UTC) == 0) {
    fprintf(stderr, "timespec_get failed");
    return -1;
  }
  srandom(ts.tv_nsec ^ ts.tv_sec);

  return (
  // clang-format off
#if defined(TEST_CASES)
      // Test Cases
      RegisterDisjointSetTestCase() != 0 +
      RegisterBubbleSortTestCase() != 0 +
      RegisterInsertionSortTestCase() != 0 +
      RegisterSortInstrumentationTestCase() != 0 +
      RegisterMatrixOperationTestCase() != 0 +
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
      RegisterEuclidDistTests() != 0 +
      RegisterClosestDistanceTests() != 0 +
      RegisterClosestDistanceSlowTests() != 0 +
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

  fprintf(stderr, "Suite not found: %s\n", suite_name);
  return NULL;
}

int TestRunner(int (*register_tests)()) {
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

  // Get this value before CU_cleanup_registry() or it will revert to zero
  int ret = CU_get_number_of_failures() != 0;

  // Clean up registry and return
  CU_cleanup_registry();
  return ret;
}

int main(void) { return TestRunner(RegisterTests); }
