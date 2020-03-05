#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/OverflowChecker.h"

#include "SequenceAlignment.h"

typedef struct SeqAlign {
  char* x;
  size_t x_n;
  char* y;
  size_t y_n;
  penalty gap_penalty;
  penalty mismatch_penalty;
} SeqAlign;

static void _printSolutions(size_t x_n, size_t y_n,
                            penalty solutions[x_n + 1][y_n + 1]) {
  printf("\n");
  for (size_t x = 0; x <= x_n; x++) {
    printf("i=%-3zu", x);
    for (size_t y = 0; y <= y_n; y++) {
      printf("%3zu ", solutions[x][y]);
    }
    printf("\n");
  }

  printf("  y= ");
  for (size_t i = 0; i <= y_n; i++) {
    printf("%3zu ", i);
  }
  printf("\n");
}

/*
 * It's assumed that <x> and <y> strings are the same length
 */
static ResultCode _seqAlignSolution_Init(char* x, char* y, penalty nw_score,
                                         SeqAlignSolution** solution) {
  if (x == NULL || y == NULL || solution == NULL) return NullParameter;
  if (*solution != NULL) return OutputPointerIsNotNull;

  SeqAlignSolution* _result = malloc(sizeof(SeqAlignSolution));
  if (_result == NULL) return FailedMemoryAllocation;

  _result->n = strlen(x);
  _result->x = x;
  _result->y = y;
  _result->nw_score = nw_score;

  *solution = _result;
  return Success;
}

static penalty _min(penalty x, penalty y, penalty z) {
  penalty _min = 0;

  _min = (x < y) ? x : y;
  _min = (_min < z) ? _min : z;

  return _min;
}

static ResultCode _reconstruct(
    SeqAlign* problem, penalty solutions[problem->x_n + 1][problem->y_n + 1],
    SeqAlignSolution** solution) {
  if (is_add_overflow_size_t(problem->x_n, problem->y_n))
    return ArithmeticOverflow;

  // Max possible size of aligned string
  size_t size = problem->y_n + problem->x_n;

  // Add 1 for null terminator
  char x[size + 1];
  char y[size + 1];

  // back fill with gaps
  memset(x, '_', size);
  memset(y, '_', size);

  // Add null terminators
  x[size] = '\0';
  y[size] = '\0';

  size_t i = problem->x_n, j = problem->y_n, xpos = size - 1, ypos = size - 1;

  while (i != 0 && j != 0) {
    char _x = problem->x[i - 1];
    char _y = problem->y[j - 1];

    if (_x == _y) {
      x[xpos--] = _x;
      y[ypos--] = _y;
      i--;
      j--;
    } else {
      // This costs us a few extra cycles, but it's far more readable and the
      // optimizer will most likely fix it anyway
      penalty case1 = solutions[i - 1][j - 1] + problem->mismatch_penalty;
      penalty case2 = solutions[i - 1][j] + problem->gap_penalty;
      penalty case3 = solutions[i][j - 1] + problem->gap_penalty;
      penalty solution = solutions[i][j];

      if (case1 == solution) {
        x[xpos--] = _x;
        y[ypos--] = _y;
        i--;
        j--;
      } else if (case2 == solution) {
        x[xpos--] = _x;
        y[ypos--] = '_';
        i--;
      } else if (case3 == solution) {
        x[xpos--] = '_';
        y[ypos--] = _y;
        j--;
      }
    }
  }

  // Copy over any characters that weren't converted by the previous loop
  while (i > 0) x[xpos--] = problem->x[--i];
  while (j > 0) y[ypos--] = problem->y[--j];

  size_t maxpos = (xpos > ypos) ? xpos : ypos;
  size_t new_size = size - maxpos;

  char* new_x = malloc(new_size);
  if (new_x == NULL) return FailedMemoryAllocation;

  char* new_y = malloc(new_size);
  if (new_y == NULL) {
    free(new_x);
    return FailedMemoryAllocation;
  }

  strncpy(new_x, &x[maxpos + 1], new_size);
  strncpy(new_y, &y[maxpos + 1], new_size);

  ResultCode code = _seqAlignSolution_Init(
      new_x, new_y, solutions[problem->x_n][problem->y_n], solution);
  if (code != Success) {
    free(new_x);
    free(new_y);
  }

  return code;
}

void SeqAlignSolution_Destory(SeqAlignSolution* self) {
  if (self == NULL) return;
  free(self->x);
  free(self->y);
  free(self);
}

ResultCode SeqAlign_Init(char* x, char* y, penalty gap_penalty,
                         penalty mismatch_penalty, SeqAlign** result) {
  if (x == NULL || y == NULL || result == NULL) return NullParameter;
  if (*result != NULL) return OutputPointerIsNotNull;

  SeqAlign* _result = malloc(sizeof(SeqAlign));
  if (_result == NULL) return FailedMemoryAllocation;

  _result->x = x;
  _result->x_n = strlen(x);
  _result->y = y;
  _result->y_n = strlen(y);
  _result->gap_penalty = gap_penalty;
  _result->mismatch_penalty = mismatch_penalty;

  *result = _result;
  return Success;
}

void SeqAlign_Destory(SeqAlign* self) { free(self); }

ResultCode SequenceAlignment_Score(SeqAlign* problem,
                                   SeqAlignSolution** solution) {
  if (problem == NULL || solution == NULL) return NullParameter;
  if (*solution != NULL) return OutputPointerIsNotNull;

  penalty solutions[problem->x_n + 1][problem->y_n + 1];

  for (size_t i = 0; i <= problem->x_n; i++) {
    if (is_mul_overflow_ulong(i, problem->gap_penalty))
      return ArithmeticOverflow;
    solutions[i][0] = i * problem->gap_penalty;
  }

  for (size_t j = 0; j <= problem->y_n; j++) {
    if (is_mul_overflow_ulong(j, problem->gap_penalty))
      return ArithmeticOverflow;
    solutions[0][j] = j * problem->gap_penalty;
  }

  for (size_t i = 1; i <= problem->x_n; i++) {
    for (size_t j = 1; j <= problem->y_n; j++) {
      char x = problem->x[i - 1];
      char y = problem->y[j - 1];

      if (x == y)
        solutions[i][j] = solutions[i - 1][j - 1];
      else {
        if (is_add_overflow_ulong(solutions[i - 1][j - 1],
                                  problem->mismatch_penalty))
          return ArithmeticOverflow;
        penalty case1 = solutions[i - 1][j - 1] + problem->mismatch_penalty;

        if (is_add_overflow_ulong(solutions[i - 1][j], problem->gap_penalty))
          return ArithmeticOverflow;
        penalty case2 = solutions[i - 1][j] + problem->gap_penalty;

        if (is_add_overflow_ulong(solutions[i][j - 1], problem->gap_penalty))
          return ArithmeticOverflow;
        penalty case3 = solutions[i][j - 1] + problem->gap_penalty;

        solutions[i][j] = _min(case1, case2, case3);
      }
    }
  }

  (void)_printSolutions;
  /* _printSolutions(problem->x_n, problem->y_n, solutions); */
  return _reconstruct(problem, solutions, solution);
}