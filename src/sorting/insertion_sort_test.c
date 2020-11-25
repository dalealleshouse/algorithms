/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "insertion_sort.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "linked_list.h"
#include "sorting_test_helpers.h"

STANDARD_SORTING_TESTS(InsertionSort)

/*******************************************************************************
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 *
 * assert is the primary form of error checking in this file and assert
 * statements are NOT evaluated for release builds. This is NOT an example of
 * code suitable for production use.
 *
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 *******************************************************************************/

typedef struct ContestantResult {
  char name[100];
  float score;
} ContestantResult;

static void ScoreTracker(LinkedList* scores, ContestantResult* cr) {
  assert(scores != NULL);
  assert(cr != NULL);

  size_t index = 0;

  LinkedListItem* item = scores->head;

  // Cycle through the list to find the ordered place for <cr>
  while (item != NULL) {
    if (cr->score <= ((ContestantResult*)item->payload)->score) break;

    index++;
    item = item->next;
  }

  // Insert <cr> at the desired location place
  ResultCode result_code = LinkedList_InsertAt(scores, cr, index);

  (void)result_code;
  assert(result_code == kSuccess);
}

static void PrintTopTen(LinkedList* scores) {
  assert(scores != NULL);

  LinkedListItem* item = scores->head;

  // Print top 10, if there are less than 10 items in the list, print them all
  for (size_t i = 0; i < 10 && item != NULL; i++) {
    ContestantResult* cr = (ContestantResult*)item->payload;
    assert(cr != NULL);

    printf("%lu %s %f \n", i + 1, cr->name, cr->score);
    item = item->next;
  }
}

static void ParseDataFile(char* file_path) {
  const size_t kBufferSize = 128;
  LinkedList* scores = NULL;

  // Stop if user does not have access to the file or it does not exist
  assert(access(file_path, R_OK) == 0);

  // Create a linked list to store scores in
  ResultCode result_code = LinkedList_Create(free, NULL, &scores);

  (void)result_code;
  assert(result_code == kSuccess);

  // Open the file
  FILE* file;
  file = fopen(file_path, "r");
  assert(file != NULL);

  char line[kBufferSize];
  char score_string[kBufferSize];
  while (fgets(line, kBufferSize, file)) {
    // Stop if there is a read error
    assert(feof(file) == 0);

    ContestantResult* cr = malloc(sizeof(ContestantResult));
    assert(cr != NULL);

    // intermediate score string to effectively test for failed conversions
    int vars_parsed = sscanf(line, "%[^\t]%s", cr->name, score_string);
    (void)vars_parsed;
    assert(vars_parsed == 2);

    cr->score = strtof(score_string, NULL);
    assert(errno == 0);

    // Add the contestant result to the running list of scores
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
