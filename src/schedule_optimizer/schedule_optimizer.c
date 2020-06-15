#include "./schedule_optimizer.h"

#include <stdlib.h>

// TODO(dalealleshouse): This entire file requires arithmetic overflow checking

double calc_score(const job* job1) { return job1->weight / job1->length; }

int ratio_compare(const void* job1, const void* job2) {
  double job1_ratio = calc_score((job*)job1);
  double job2_ratio = calc_score((job*)job2);
  double diff;

  if (job1_ratio == job2_ratio) {
    diff = ((job*)job1)->weight - ((job*)job2)->weight;
  } else {
    diff = job1_ratio - job2_ratio;
  }

  if (diff < 0.0) {
    return 1;
  } else if (diff > 0.0) {
    return -1;
  } else {
    return 0;
  }
}

Result ScheduleOptimizer_Calc(job* jobs, const size_t n,
                              double* optimization_score) {
  if (jobs == NULL || optimization_score == NULL) return kNullParameter;

  if (n <= 0) return kEmpty;

  double running_length = 0;
  *optimization_score = 0;
  qsort(jobs, n, sizeof(job), ratio_compare);

  for (size_t i = 0; i < n; i++) {
    running_length = running_length + jobs->length;

    *optimization_score = *optimization_score + (running_length * jobs->weight);

    jobs++;
  }

  return kSuccess;
}
