#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "ScheduleOptimizer.h"
#include "include/TestHelpers.h"

const double EPSILON = 0.0000000000000000001;

static void ScheduleOptimizer_Calc_null_parameter()
{
    double score = 0;
    Result result = ScheduleOptimizer_Calc(NULL, 5, &score);
    CU_ASSERT_EQUAL(NullParameter, result);

    job jobs = { 1.0, 1.0 };
    result = ScheduleOptimizer_Calc(&jobs, 1, NULL);
    CU_ASSERT_EQUAL(NullParameter, result);
}

static void ScheduleOptimizer_Calc_same_lengths()
{
    const double expected_score = 275.0;
    job expected[] = { { 5, 20 }, { 5, 10 }, { 5, 5 } };

    double score;
    job jobs[] = { { 5, 5 }, { 5, 10 }, { 5, 20 } };
    size_t n = sizeof(jobs) / sizeof(jobs[0]);

    Result result = ScheduleOptimizer_Calc(jobs, n, &score);
    int cmp_result = memcmp(&jobs, &expected, sizeof(jobs));

    CU_ASSERT_DOUBLE_EQUAL(score, expected_score, EPSILON);
    CU_ASSERT_EQUAL(0, cmp_result);
    CU_ASSERT_EQUAL(Success, result);
}

static void ScheduleOptimizer_Calc_same_weights()
{
    const double expected_score = 250.0;
    job expected[] = { { 5, 5 }, { 10, 5 }, { 15, 5 } };

    double score;
    job jobs[] = { { 15, 5 }, { 5, 5 }, { 10, 5 } };
    size_t n = sizeof(jobs) / sizeof(jobs[0]);

    Result result = ScheduleOptimizer_Calc(jobs, n, &score);
    int cmp_result = memcmp(&jobs, &expected, sizeof(jobs));

    CU_ASSERT_DOUBLE_EQUAL(score, expected_score, EPSILON);
    CU_ASSERT_EQUAL(0, cmp_result);
    CU_ASSERT_EQUAL(Success, result);
}

static void ScheduleOptimizer_Calc_empty_array()
{
    double score;
    job jobs[1];
    size_t n = 0;

    Result result = ScheduleOptimizer_Calc(jobs, n, &score);
    CU_ASSERT_EQUAL(Empty, result);
}

int register_schedule_optimizer_tests()
{
    CU_TestInfo Calc_tests[] = { CU_TEST_INFO(
                                     ScheduleOptimizer_Calc_null_parameter),
        CU_TEST_INFO(ScheduleOptimizer_Calc_same_lengths),
        CU_TEST_INFO(ScheduleOptimizer_Calc_same_weights),
        CU_TEST_INFO(ScheduleOptimizer_Calc_empty_array), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "ScheduleOptimizer_Calc",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Calc_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
