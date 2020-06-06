#pragma once

#include <stddef.h>
#include "../utils/result_code.h"

typedef struct job {
  double length;
  double weight;
} job;

/*
 * Calcs the supplied array of jobs into an optimial schedule
 *
 * params:
 *  jobs: Array of jobs structs
 *  n: Number of elements in the array
 *  optimization_score: Will be populated with the calculated optimization_score
 *
 * side effects:
 *  job array will be sorted in place
 *  optimization_score will be populated
 *
 * returns:
 *  Result code
 */
Result ScheduleOptimizer_Calc(job* jobs, const size_t n,
                              double* optimization_score);
