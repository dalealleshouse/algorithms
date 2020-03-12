#pragma once

#include "include/ResultCode.h"

typedef unsigned long penalty;
const char GAP;

/*
 * Implementation is private to ensure it is not modified after initialization
 */
typedef struct SeqAlign SeqAlign;

/*
 * WARNING: providing a non-null terminated string for the values of <x> or <y>
 * will result in an undefined behavior
 *
 * For demonstration sake, we are assuming that all mismatched characters have
 * the same penalty. However, this is typically not the case in the real world.
 */
ResultCode SeqAlign_Init(char* x, char* y, penalty gap_penalty,
                         penalty mismatch_penalty, SeqAlign** result);
void SeqAlign_Destory(SeqAlign* self);

typedef struct SeqAlignSolution {
  size_t n;
  char* x;
  char* y;
  penalty nw_score;
} SeqAlignSolution;

void SeqAlignSolution_Destory(SeqAlignSolution* self);

ResultCode SequenceAlignment_Score(SeqAlign* problem,
                                   SeqAlignSolution** solution);
