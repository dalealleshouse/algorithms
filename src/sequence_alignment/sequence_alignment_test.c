#include "sequence_alignment.h"

#include <limits.h>
#include <stdlib.h>

#include "CUnit/CUnit.h"
#include "test_helpers.h"

static void _testSequenceAlignment_Score(char* x, char* y, penalty gap,
                                         penalty mismatch, penalty nw, size_t s,
                                         char* expected_x, char* expected_y) {
  SeqAlignSolution* solution = NULL;
  SeqAlign* problem = NULL;
  ResultCode code = SeqAlign_Init(x, y, gap, mismatch, &problem);
  CU_ASSERT_EQUAL(code, kSuccess);

  code = SequenceAlignment_Score(problem, &solution);

  CU_ASSERT_EQUAL(code, kSuccess);
  CU_ASSERT_EQUAL(solution->nw_score, nw);
  CU_ASSERT_EQUAL(solution->n, s);
  CU_ASSERT_STRING_EQUAL(solution->x, expected_x);
  CU_ASSERT_STRING_EQUAL(solution->y, expected_y);

  SeqAlignSolution_Destory(solution);
  SeqAlign_Destory(problem);
}

static void SequenceAlignment_Score_kNullParameter() {
  ResultCode code = SequenceAlignment_Score(NULL, NULL);
  CU_ASSERT_EQUAL(code, kNullParameter);
}

static void SequenceAlignment_Score_InvalidInput() {
  SeqAlignSolution* solution = malloc(sizeof(SeqAlignSolution));
  SeqAlign* problem = NULL;
  ResultCode code = SeqAlign_Init("x", "y", 5, 5, &problem);
  CU_ASSERT_EQUAL(code, kSuccess);

  code = SequenceAlignment_Score(problem, &solution);

  CU_ASSERT_EQUAL(code, kOutputPointerIsNotNull);
  free(solution);
  SeqAlign_Destory(problem);
}

static void SequenceAlignment_Score_kArithmeticOverflow() {
  SeqAlign* problem = NULL;
  ResultCode code = SeqAlign_Init("and", "not", ULONG_MAX, ULONG_MAX, &problem);
  CU_ASSERT_EQUAL(code, kSuccess);

  SeqAlignSolution* result = NULL;
  code = SequenceAlignment_Score(problem, &result);

  CU_ASSERT_EQUAL(code, kArithmeticOverflow);
  SeqAlign_Destory(problem);
}

static void SequenceAlignment_Score_HappyPath() {
  _testSequenceAlignment_Score("AGTACG", "ACATAG", 1, 2, 4, 7, "A_GTACG",
                               "ACATA_G");
}

static void SequenceAlignment_Score_DifferentSizes() {
  _testSequenceAlignment_Score("AGGGCT", "AGGCA", 2, 3, 5, 6, "AGGGCT",
                               "A_GGCA");
}

static void SequenceAlignment_Score_kEmptyString() {
  _testSequenceAlignment_Score("AGGGCT", "", 2, 3, 12, 6, "AGGGCT", "______");
}

static void SequenceAlignment_Score_OtherkEmptyString() {
  _testSequenceAlignment_Score("", "AGGGCT", 2, 3, 12, 6, "______", "AGGGCT");
}

int RegisterSequenceAlignmentTests() {
  CU_TestInfo SequenceAlignment_Tests[] = {
      CU_TEST_INFO(SequenceAlignment_Score_kNullParameter),
      CU_TEST_INFO(SequenceAlignment_Score_InvalidInput),
      CU_TEST_INFO(SequenceAlignment_Score_kArithmeticOverflow),
      CU_TEST_INFO(SequenceAlignment_Score_HappyPath),
      CU_TEST_INFO(SequenceAlignment_Score_DifferentSizes),
      CU_TEST_INFO(SequenceAlignment_Score_kEmptyString),
      CU_TEST_INFO(SequenceAlignment_Score_OtherkEmptyString),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Sequence Alignment",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = SequenceAlignment_Tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
