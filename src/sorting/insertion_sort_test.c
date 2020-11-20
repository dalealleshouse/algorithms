/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "insertion_sort.h"

#include <stdbool.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "linked_list.h"
#include "sorting_test_helpers.h"

STANDARD_SORTING_TESTS(InsertionSort)

/*******************************************************************************
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 *
 * There is no error checking to make the code more readable. This should not be
 * considered an example of best practices.
 *
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 *******************************************************************************/

typedef struct ContestantResult {
  char name[100];
  float score;
} ContestantResult;

int DescendingScoreStrategy(const void* x, const void* y) {
  ContestantResult* _x = (ContestantResult*)x;
  ContestantResult* _y = (ContestantResult*)y;

  if (_y->score > _x->score) return 1;

  return -1;
}

void ScoreTracker(LinkedList* scores, ContestantResult* cr) {
  size_t index = 0;

  LinkedListItem* item = scores->head;
  while (item != NULL) {
    if (DescendingScoreStrategy(cr, item->payload) <= 0) break;

    index++;
    item = item->next;
  }

  LinkedList_InsertAt(scores, cr, index);
}

void PrintTopTen(LinkedList* scores) {
  LinkedListItem* item = scores->tail;

  for (size_t i = 0; i < 10; i++) {
    ContestantResult* cr = (ContestantResult*)item->payload;
    printf("%lu %s %f \n", i + 1, cr->name, cr->score);
    item = item->prev;
  }
}

void ParseDataFile(char* file_path) {
  const size_t kBufferSize = 128;
  LinkedList* scores = NULL;

  LinkedList_Create(free, NULL, &scores);

  FILE* file;
  file = fopen(file_path, "r");

  char line[kBufferSize];
  while (fgets(line, kBufferSize, file)) {
    ContestantResult* cr = malloc(sizeof(ContestantResult));
    sscanf(line, "%[^\t]%f", cr->name, &cr->score);
    ScoreTracker(scores, cr);
  }

  fclose(file);

  PrintTopTen(scores);
  LinkedList_Destroy(scores);
}

static void InsertionSortTestCase() {
  ParseDataFile("src/sorting/test_data/super_sport.txt");
}

int RegisterInsertionSortTestCase() {
  CU_TestInfo bubble_sort_test_case[] = {CU_TEST_INFO(InsertionSortTestCase),
                                         CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Insertion Sort Test Case",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = bubble_sort_test_case},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
