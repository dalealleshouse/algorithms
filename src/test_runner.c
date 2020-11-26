/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

// utils
extern int RegisterGraphTests();
extern int RegisterOverflowTests();
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
extern int RegisterPartitionTests();

// Inversion Couht
extern int RegisterInversionCountTests();

// Matrix Multiplication
extern int RegisterIsPowerOfTwoTests();
extern int RegisterMultiplySquareMatricesTests();

// Quick Select
extern int RegisterQuickSelectPartitionTests();

// Cache
extern int RegisterCacheTests();

extern int RegisterBitVectorTests();

int RegisterTests() {
  return (
             // clang-format off
      // Test Cases
      /* RegisterDisjointSetTestCase() != 0 + */
      /* RegisterBubbleSortTestCase() != 0 + */
      /* RegisterInsertionSortTestCase() != 0 + */
      RegisterSortInstrumentationTestCase() != 0 +

      // Unit tests
      /* RegisterGraphTests() != 0 + */
      /* RegisterOverflowTests() != 0 + */
      /* RegisterMathTests() != 0 + */
      /* RegisterCommonTests() != 0 + */
      /* RegisterGraphSearchTests() != 0 + */
      /* RegisterQueueTests() != 0 + */
      /* RegisterStackTests() != 0 + */
      /* RegisterHeapTests() != 0 + */
      /* RegisterHashTableTests() != 0 + */
      /* RegisterBloomFilterTests() != 0 + */
      /* RegisterHashFunctionTests() != 0 + */
      /* RegisterDisjointSetTests() != 0 + */
      /* RegisterKnapsackProblemTests() != 0 + */
      /* RegisterRunningMedianTests() != 0 + */
      /* RegisterSequenceAlignmentTests() != 0 + */
      /* RegisterDijkstraTests() != 0 + */
      /* RegisterShortestPathTests() != 0 + */
      /* RegisterBellmanFordTests() != 0 + */
      /* RegisterFloydWarshallTests() != 0 + */
      /* RegisterHuffmanCodeTests() != 0 + */
      /* RegisterLinkedListTests() != 0 + */
      /* RegisterArrayTests() != 0 + */
      /* RegisterSortedArrayTests() != 0 + */
      /* RegisterBinaryTreeTests() != 0 + */
      /* RegisterGraphTests() != 0 + */
      /* RegisterMinCutTests() != 0 + */
      /* RegisterSatisfiabilityTests() != 0 + */
      /* RegisterQuickSortTests() != 0 + */
      /* RegisterExtraQuickSortTests() != 0 + */
      /* RegisterPartitionTests() != 0 + */
      /* RegisterBubbleSortTests() != 0 + */
      /* RegisterInsertionSortTests() != 0 + */
      /* RegisterSelectionSortTests() != 0 + */
      /* RegisterMergeSortAdapterTests() != 0 + */
      /* RegisterTravelingSalesmanTests() != 0 + */
      /* RegisterWeightedIndependentSetTests() != 0 + */
      /* RegisterMallocTestWrapperTests() != 0 + */
      /* RegisterMinSpanningTreeTests() != 0 + */
      /* RegisterKruskalClusterTests() != 0 + */
      /* RegisterEuclidDistTests() != 0 + */
      /* RegisterClosestDistanceTests() != 0 + */
      /* RegisterClosestDistanceSlowTests() != 0 + */
      /* RegisterSelectTests() != 0 + */
      /* RegisterQuickSelectTests() != 0 + */
      /* RegisterPartitionTests() != 0 + */
      /* RegisterInversionCountTests() != 0 + */
      /* RegisterIsPowerOfTwoTests() != 0 + */
      /* RegisterMultiplySquareMatricesTests() != 0 + */
      /* RegisterQuickSelectPartitionTests() != 0 + */
      /* RegisterCacheTests() != 0 + */
      /* RegisterBitVectorTests() != 0 + */
      0) * -1;
  // clang-format on
}

int TestRunner(int (*register_tests)()) {
  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

  if (register_tests() != 0) {
    CU_cleanup_registry();
    return -1;
  }

  // Run all tests using the basic interface
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();

  // Get this value before CU_cleanup_registry() or it will revert to zero
  int ret = CU_get_number_of_failures() != 0;

  // Clean up registry and return
  CU_cleanup_registry();
  return ret;
}

int main(void) { return TestRunner(RegisterTests); }
