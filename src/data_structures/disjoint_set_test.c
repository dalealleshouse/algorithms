#include "./disjoint_set.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "../utils/malloc_test_wrapper.h"
#include "../utils/result_code.h"
#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#define SUT(size, code_block)                   \
  {                                             \
    DisjointSet sut;                            \
    Result result = DisjointSet_Init(&sut, 10); \
    CU_ASSERT_EQUAL(result, Success);           \
                                                \
    code_block;                                 \
                                                \
    DisjointSet_Destory(&sut);                  \
  }

/*******************************************************************************
 * DisjointSet_Init
 ******************************************************************************/
static void DisjointSet_Init_null_parameter() {
  Result result = DisjointSet_Init(NULL, 0);
  CU_ASSERT_EQUAL(result, NullParameter);
}

static void DisjointSet_Init_invalid_size() {
  DisjointSet sut;

  Result result = DisjointSet_Init(&sut, 0);
  CU_ASSERT_EQUAL(result, ArgumentOutOfRange);
}

static void DisjointSet_Init_hash_table() {
  SUT(10, {
    CU_ASSERT_PTR_NOT_NULL(sut.items);
    CU_ASSERT_EQUAL(HashTable_GetN(sut.items), 0);
  });
}

static void DisjointSet_Init_num_sets() {
  SUT(10, { CU_ASSERT_EQUAL(sut.num_sets, 0); });
}

/*******************************************************************************
 * DisjointSet_MakeSet
 ******************************************************************************/
static void DisjointSet_MakeSet_null_parameter() {
  int some_pointer;
  SetItem* set;

  Result _result = DisjointSet_MakeSet(NULL, NULL, NULL);
  CU_ASSERT_EQUAL(_result, NullParameter);

  _result = DisjointSet_MakeSet(NULL, &some_pointer, &set);
  CU_ASSERT_EQUAL(_result, NullParameter);

  SUT(10, {
    _result = DisjointSet_MakeSet(&sut, NULL, NULL);
    CU_ASSERT_EQUAL(_result, NullParameter);

    _result = DisjointSet_MakeSet(&sut, &some_pointer, NULL);
    CU_ASSERT_EQUAL(_result, NullParameter);

    _result = DisjointSet_MakeSet(&sut, NULL, &set);
    CU_ASSERT_EQUAL(_result, NullParameter);
  });
}

static void DisjointSet_MakeSet_item_exists() {
  SUT(10, {
    int some_pointer;
    SetItem* set;
    Result _result = DisjointSet_MakeSet(&sut, &some_pointer, &set);
    CU_ASSERT_EQUAL(_result, Success);

    _result = DisjointSet_MakeSet(&sut, &some_pointer, &set);
    CU_ASSERT_EQUAL(_result, Duplicate);
  });
}

static void DisjointSet_MakeSet_failed_malloc() {
  int some_pointer = 1;
  SetItem* item = NULL;
  SUT(10, {
    FAILED_MALLOC_TEST({
      Result _result = DisjointSet_MakeSet(&sut, &some_pointer, &item);
      CU_ASSERT_EQUAL(_result, FailedMemoryAllocation);
    });
  });
}

static void DisjointSet_MakeSet_inits_set_values() {
  SUT(10, {
    int some_pointer;
    SetItem* set;
    Result _result = DisjointSet_MakeSet(&sut, &some_pointer, &set);
    CU_ASSERT_EQUAL(_result, Success);

    CU_ASSERT_PTR_EQUAL(set->parent, set);
    CU_ASSERT_PTR_EQUAL(set->payload, &some_pointer);
    CU_ASSERT_EQUAL(set->rank, 0);
  });
}

static void DisjointSet_MakeSet_increments_num_sets() {
  SUT(10, {
    CU_ASSERT_EQUAL(sut.num_sets, 0);
    int some_pointer;
    SetItem* set;
    Result _result = DisjointSet_MakeSet(&sut, &some_pointer, &set);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_EQUAL(sut.num_sets, 1);
  });
}

/*******************************************************************************
 * DisjointSet_FindSet
 ******************************************************************************/
static void DisjointSet_FindSet_null_parameter() {
  int some_pointer;
  SetItem* set;

  Result _result = DisjointSet_FindSet(NULL, NULL, NULL);
  CU_ASSERT_EQUAL(_result, NullParameter);

  _result = DisjointSet_FindSet(NULL, &some_pointer, &set);
  CU_ASSERT_EQUAL(_result, NullParameter);

  SUT(10, {
    _result = DisjointSet_FindSet(&sut, NULL, NULL);
    CU_ASSERT_EQUAL(_result, NullParameter);

    _result = DisjointSet_FindSet(&sut, &some_pointer, NULL);
    CU_ASSERT_EQUAL(_result, NullParameter);

    _result = DisjointSet_FindSet(&sut, NULL, &set);
    CU_ASSERT_EQUAL(_result, NullParameter);
  });
}

static void DisjointSet_FindSet_not_found() {
  SUT(10, {
    int some_pointer;
    SetItem* set;
    Result _result = DisjointSet_FindSet(&sut, &some_pointer, &set);
    CU_ASSERT_EQUAL(_result, NotFound);
  });
}

static void DisjointSet_FindSet_returns_self() {
  SUT(10, {
    int some_pointer;
    SetItem* set = NULL;
    Result _result = DisjointSet_MakeSet(&sut, &some_pointer, &set);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_PTR_NOT_NULL(set);

    SetItem* found_set = NULL;
    _result = DisjointSet_FindSet(&sut, &some_pointer, &found_set);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_PTR_EQUAL(set, found_set);
  });
}

static size_t _getPathLength(SetItem* item) {
  size_t count = 0;

  while (item != item->parent) {
    item = item->parent;
    ++count;
  }

  return count;
}

static void DisjointSet_FindSet_compresses_paths() {
  int vals[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  SetItem* rep = NULL;
  SetItem* set = NULL;
  Result _result;

  SUT(10, {
    for (size_t i = 0; i < 10; i++) {
      _result = DisjointSet_MakeSet(&sut, &vals[i], &set);
      CU_ASSERT_EQUAL(_result, Success);
      CU_ASSERT_PTR_NOT_NULL(set);
    }

    for (size_t i = 1; i < 10; i++) {
      _result = DisjointSet_Union(&sut, &vals[i - 1], &vals[i]);
      CU_ASSERT_EQUAL(_result, Success);
    }

    // Find the representative set
    _result = DisjointSet_FindSet(&sut, &vals[0], &rep);
    CU_ASSERT_EQUAL(_result, Success);

    // If the paths are properly compressed, the rep will have a path length
    // of 0 and all others will have a path length of 1
    for (size_t i = 0; i < 10; i++) {
      uintptr_t val = (uintptr_t)&vals[i];
      set = HashTable_Find(sut.items, &val, sizeof(void*));
      size_t path_len = _getPathLength(set);

      if (set == rep) {
        CU_ASSERT_EQUAL(path_len, 0);
      } else {
        CU_ASSERT_EQUAL(path_len, 1);
      }
    }
  });
}

/*******************************************************************************
 * DisjointSet_Union
 ******************************************************************************/
static void DisjointSet_Union_null_parameter() {
  int x = 1, y = 2;

  Result _result = DisjointSet_Union(NULL, NULL, NULL);
  CU_ASSERT_EQUAL(_result, NullParameter);

  _result = DisjointSet_Union(NULL, &x, &y);
  CU_ASSERT_EQUAL(_result, NullParameter);

  SUT(10, {
    _result = DisjointSet_Union(&sut, NULL, NULL);
    CU_ASSERT_EQUAL(_result, NullParameter);

    _result = DisjointSet_Union(&sut, &x, NULL);
    CU_ASSERT_EQUAL(_result, NullParameter);

    _result = DisjointSet_Union(&sut, NULL, &y);
    CU_ASSERT_EQUAL(_result, NullParameter);
  });
}

static void DisjointSet_Union_not_found() {
  int x = 1, y = 2;
  SetItem* set;

  SUT(10, {
    Result _result = DisjointSet_Union(&sut, &x, &y);
    CU_ASSERT_EQUAL(_result, NotFound);

    _result = DisjointSet_MakeSet(&sut, &x, &set);
    CU_ASSERT_EQUAL(_result, Success);

    _result = DisjointSet_Union(&sut, &x, &y);
    CU_ASSERT_EQUAL(_result, NotFound);

    _result = DisjointSet_Union(&sut, &y, &x);
    CU_ASSERT_EQUAL(_result, NotFound);
  });
}

static void DisjointSet_Union_unions_sets() {
  int x = 1, y = 2;
  SetItem* x_set = NULL;
  SetItem* y_set = NULL;

  SUT(10, {
    Result _result = DisjointSet_MakeSet(&sut, &x, &x_set);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_PTR_NOT_NULL(x_set);

    _result = DisjointSet_MakeSet(&sut, &y, &y_set);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_PTR_NOT_NULL(y_set);

    CU_ASSERT_EQUAL(sut.num_sets, 2);

    _result = DisjointSet_Union(&sut, &x, &y);
    CU_ASSERT_EQUAL(_result, Success);

    x_set = NULL;
    y_set = NULL;

    _result = DisjointSet_FindSet(&sut, &x, &x_set);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_PTR_NOT_NULL(x_set);

    _result = DisjointSet_FindSet(&sut, &y, &y_set);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_PTR_NOT_NULL(y_set);

    CU_ASSERT_PTR_EQUAL(x_set, y_set);
    CU_ASSERT_EQUAL(sut.num_sets, 1);
  });
}

static void DisjointSet_Union_unions_several() {
  int vals[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  SetItem* set = NULL;
  SetItem* should = NULL;
  Result _result;

  SUT(10, {
    for (size_t i = 0; i < 10; i++) {
      _result = DisjointSet_MakeSet(&sut, &vals[i], &set);
      CU_ASSERT_EQUAL(_result, Success);
      CU_ASSERT_PTR_NOT_NULL(set);
    }

    CU_ASSERT_EQUAL(sut.num_sets, 10);

    for (size_t i = 1; i < 10; i++) {
      _result = DisjointSet_Union(&sut, &vals[i - 1], &vals[i]);
      CU_ASSERT_EQUAL(_result, Success);
    }

    CU_ASSERT_EQUAL(sut.num_sets, 1);
    _result = DisjointSet_FindSet(&sut, &vals[0], &should);

    for (size_t i = 0; i < 10; i++) {
      _result = DisjointSet_FindSet(&sut, &vals[i], &set);
      CU_ASSERT_EQUAL(_result, Success);
      CU_ASSERT_PTR_EQUAL(set, should);
    }
  });
}

static void DisjointSet_Union_two_sets() {
  int vals[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  SetItem* set = NULL;
  SetItem* should = NULL;
  Result _result;

  SUT(10, {
    for (size_t i = 0; i < 10; i++) {
      _result = DisjointSet_MakeSet(&sut, &vals[i], &set);
      CU_ASSERT_EQUAL(_result, Success);
      CU_ASSERT_PTR_NOT_NULL(set);
    }

    CU_ASSERT_EQUAL(sut.num_sets, 10);

    for (size_t i = 1; i < 5; i++) {
      _result = DisjointSet_Union(&sut, &vals[i - 1], &vals[i]);
      CU_ASSERT_EQUAL(_result, Success);
    }

    for (size_t i = 6; i < 10; i++) {
      _result = DisjointSet_Union(&sut, &vals[i - 1], &vals[i]);
      CU_ASSERT_EQUAL(_result, Success);
    }

    CU_ASSERT_EQUAL(sut.num_sets, 2);

    _result = DisjointSet_FindSet(&sut, &vals[0], &should);
    for (size_t i = 0; i < 5; i++) {
      _result = DisjointSet_FindSet(&sut, &vals[i], &set);
      CU_ASSERT_EQUAL(_result, Success);
      CU_ASSERT_PTR_EQUAL(set, should);
    }

    _result = DisjointSet_FindSet(&sut, &vals[5], &should);
    for (size_t i = 6; i < 10; i++) {
      _result = DisjointSet_FindSet(&sut, &vals[i], &set);
      CU_ASSERT_EQUAL(_result, Success);
      CU_ASSERT_PTR_EQUAL(set, should);
    }
  });
}

static void DisjointSet_Union_ignores_already_joined() {
  int x = 1, y = 2;
  SetItem* x_set = NULL;
  SetItem* y_set = NULL;

  SUT(10, {
    Result _result = DisjointSet_MakeSet(&sut, &x, &x_set);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_PTR_NOT_NULL(x_set);

    _result = DisjointSet_MakeSet(&sut, &y, &y_set);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_PTR_NOT_NULL(y_set);

    _result = DisjointSet_Union(&sut, &x, &y);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_EQUAL(sut.num_sets, 1);

    _result = DisjointSet_Union(&sut, &x, &y);
    CU_ASSERT_EQUAL(_result, Success);
    CU_ASSERT_EQUAL(sut.num_sets, 1);
  });
}

/*******************************************************************************
 * DisjointSet_Destroy
 ******************************************************************************/
static void DisjointSet_Destory_null_parameter() { DisjointSet_Destory(NULL); }

int RegisterDisjointSetTests() {
  CU_TestInfo DisjointSetTests[] = {
      CU_TEST_INFO(DisjointSet_Init_null_parameter),
      CU_TEST_INFO(DisjointSet_Init_invalid_size),
      CU_TEST_INFO(DisjointSet_Init_hash_table),
      CU_TEST_INFO(DisjointSet_Init_num_sets),
      CU_TEST_INFO(DisjointSet_MakeSet_null_parameter),
      CU_TEST_INFO(DisjointSet_MakeSet_item_exists),
      CU_TEST_INFO(DisjointSet_MakeSet_failed_malloc),
      CU_TEST_INFO(DisjointSet_MakeSet_inits_set_values),
      CU_TEST_INFO(DisjointSet_MakeSet_increments_num_sets),
      CU_TEST_INFO(DisjointSet_FindSet_null_parameter),
      CU_TEST_INFO(DisjointSet_FindSet_not_found),
      CU_TEST_INFO(DisjointSet_FindSet_returns_self),
      CU_TEST_INFO(DisjointSet_FindSet_compresses_paths),
      CU_TEST_INFO(DisjointSet_Union_null_parameter),
      CU_TEST_INFO(DisjointSet_Union_not_found),
      CU_TEST_INFO(DisjointSet_Union_unions_sets),
      CU_TEST_INFO(DisjointSet_Union_unions_several),
      CU_TEST_INFO(DisjointSet_Union_two_sets),
      CU_TEST_INFO(DisjointSet_Union_ignores_already_joined),
      CU_TEST_INFO(DisjointSet_Destory_null_parameter),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Disjoint Set",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = DisjointSetTests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
