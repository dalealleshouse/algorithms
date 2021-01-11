/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "comparators.h"

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "test_helpers.h"

static void PIntComparator_ReturnsZeroWhenEqual() {
  int x = 1;
  int y = 1;

  int result = PIntComparator(&x, &y);
  CU_ASSERT_EQUAL(0, result);
}

static void PIntComparator_ReturnsNegativeWhenXLessThanY() {
  int x = -10;
  int y = 10;

  int result = PIntComparator(&x, &y);
  CU_ASSERT(result < 0);
}

static void PIntComparator_ReturnsOneWhenXGreaterThanY() {
  int x = 10;
  int y = -10;

  int result = PIntComparator(&x, &y);
  CU_ASSERT(result > 0);
}

int RegisterCommonTests() {
  CU_TestInfo comparator_tests[] = {
      CU_TEST_INFO(PIntComparator_ReturnsZeroWhenEqual),
      CU_TEST_INFO(PIntComparator_ReturnsNegativeWhenXLessThanY),
      CU_TEST_INFO(PIntComparator_ReturnsOneWhenXGreaterThanY),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "common - comparators",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = comparator_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
