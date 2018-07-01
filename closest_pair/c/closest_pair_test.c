#include <float.h>
#include <stdlib.h>
#include <time.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "closest_pair.h"

const double tolerance_delta = 0.000001;

double drand(double low, double high)
{
    return ((double)rand() * (high - low)) / (double)RAND_MAX + low;
}

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* eclid_dist ****************/

void euclid_dist_null_does_not_throw()
{
    int result = euclid_dist(NULL, NULL, NULL);
    CU_ASSERT_EQUAL(-1, result);
}

void euclid_dist_distance()
{
    double dist;
    point_t p1 = { 1.5, -1.5 };
    point_t p2 = { 7.5, -7.5 };

    int result = euclid_dist(&p1, &p2, &dist);
    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_DOUBLE_EQUAL(dist, 8.485281, 0.000001);
}

void euclid_dist_zero_distance()
{
    double dist;
    point_t p1 = { 1.5, -1.5 };
    point_t p2 = { 1.5, -1.5 };

    int result = euclid_dist(&p1, &p2, &dist);
    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_DOUBLE_EQUAL(dist, 0, 0.000001);
}

void euclid_dist_overflow()
{
    double dist;
    point_t p1 = { DBL_MAX, DBL_MAX };
    point_t p2 = { DBL_MIN, DBL_MIN };

    int result = euclid_dist(&p1, &p2, &dist);
    CU_ASSERT_EQUAL(-2, result);
    CU_ASSERT(isinf(dist))
}

int euclid_dist_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("euclid dist suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        return -1;
    }

    /* add the tests to the suite */
    if ((NULL
                == CU_add_test(pSuite, "null does not throw",
                       euclid_dist_null_does_not_throw)
            || NULL
                == CU_add_test(
                       pSuite, "calculates distance", euclid_dist_distance)
            || NULL
                == CU_add_test(pSuite, "calculates zero distance",
                       euclid_dist_zero_distance)
            || NULL == CU_add_test(pSuite, "overflow", euclid_dist_overflow))

    ) {
        return CU_get_error();
    }

    return 0;
}

/************* closest distance ****************/
void closest_distance_null()
{
    int result = closest_distance(1, NULL, NULL);
    CU_ASSERT_EQUAL(-1, result);
}

void closest_distance_standard()
{
    point_t points[]
        = { { 2, 3 }, { 12, 30 }, { 40, 50 }, { 5, 1 }, { 12, 10 }, { 3, 4 } };
    const int n = sizeof(points) / sizeof(points[0]);
    point_dist_t answer = { .dist = 0 };

    int result = closest_distance(n, points, &answer);
    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_DOUBLE_EQUAL(answer.dist, 1.414214, tolerance_delta);
    CU_ASSERT(memcmp(&points[0], &answer.p1, sizeof(point_t)) == 0);
    CU_ASSERT(memcmp(&points[5], &answer.p2, sizeof(point_t)) == 0);
}

void closest_distance_standard2()
{
    const point_t points[] = { { 0, 0 }, { -2, 0 }, { 4, 0 }, { 1, 1 },
        { 3, 3 }, { -3, 2 }, { 5, 2 } };
    const int n = sizeof(points) / sizeof(points[0]);
    point_dist_t answer = { .dist = 0 };

    int result = closest_distance(n, points, &answer);
    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_DOUBLE_EQUAL(answer.dist, 1.414214, tolerance_delta);
    CU_ASSERT(memcmp(&points[0], &answer.p1, sizeof(point_t)) == 0);
    CU_ASSERT(memcmp(&points[3], &answer.p2, sizeof(point_t)) == 0);
}

void closest_distance_matches_slow()
{
    size_t n = 10000;
    point_t points[n];

    for (size_t i = 0; i < n; i++) {
        points[i].x = drand(-50, 50);
        points[i].y = drand(-50, 50);
    }

    point_dist_t slow = { .dist = 0 };
    point_dist_t fast = { .dist = 0 };

    int result = closest_slow(n, points, &slow);
    CU_ASSERT_EQUAL(0, result);

    result = closest_distance(n, points, &fast);
    CU_ASSERT_EQUAL(0, result);

    CU_ASSERT_DOUBLE_EQUAL(slow.dist, fast.dist, tolerance_delta);

    CU_ASSERT(memcmp(&slow.p1, &fast.p1, sizeof(point_t)) == 0
        || memcmp(&slow.p1, &fast.p2, sizeof(point_t)) == 0);

    CU_ASSERT(memcmp(&slow.p2, &fast.p1, sizeof(point_t)) == 0
        || memcmp(&slow.p2, &fast.p2, sizeof(point_t)) == 0);
}

void print_run_times(size_t n)
{
    point_t points[n];

    for (size_t i = 0; i < n; i++) {
        points[i].x = drand(-50, 50);
        points[i].y = drand(-50, 50);
    }

    point_dist_t slow = { .dist = 0 };
    point_dist_t fast = { .dist = 0 };

    printf("\nPrinting run times for n = %zu", n);

    clock_t begin = clock();
    int result = closest_slow(n, points, &slow);
    clock_t end = clock();

    double slow_run_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\nclosest_slow run time = %f", slow_run_time);

    begin = clock();
    result = closest_distance(n, points, &fast);
    end = clock();

    double fast_run_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\nclosest_distance run time = %f\ntime diff =", fast_run_time);

    double diff = slow_run_time - fast_run_time;

    if (diff < 0)
        printf("\x1b[31m");
    else
        printf("\x1b[32m");

    printf(" %f secs \n \x1b[0m", diff);
}

void closest_time()
{
    print_run_times(10);
    print_run_times(100);
    print_run_times(1000);
    print_run_times(10000);
    print_run_times(100000);
}
// point array has null values
// matching points
//
// point array has matching x values

int closest_distance_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("closest distance suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        return -1;
    }

    /* add the tests to the suite */
    if ((NULL
                == CU_add_test(
                       pSuite, "null does not throw", closest_distance_null)
            || NULL
                == CU_add_test(
                       pSuite, "standard case", closest_distance_standard)
            || NULL
                == CU_add_test(
                       pSuite, "standard case 2", closest_distance_standard2)
            || NULL
                == CU_add_test(pSuite, "large random array matches slow result",
                       closest_distance_matches_slow)

            || NULL == CU_add_test(pSuite, "compare times", closest_time))

    ) {
        return CU_get_error();
    }

    return 0;
}

/************* closest distance brute force ****************/
void closest_slow_null()
{
    int result = closest_slow(1, NULL, NULL);
    CU_ASSERT_EQUAL(-1, result);
}

void closest_slow_standard()
{
    const point_t points[]
        = { { 2, 3 }, { 12, 30 }, { 40, 50 }, { 5, 1 }, { 12, 10 }, { 3, 4 } };
    const int n = sizeof(points) / sizeof(points[0]);
    point_dist_t answer = { .dist = 0 };

    int result = closest_slow(n, points, &answer);
    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_DOUBLE_EQUAL(answer.dist, 1.414214, tolerance_delta);
    CU_ASSERT(memcmp(&points[0], &answer.p1, sizeof(point_t)) == 0);
    CU_ASSERT(memcmp(&points[5], &answer.p2, sizeof(point_t)) == 0);
}

void closest_slow_standard2()
{
    const point_t points[] = { { 0, 0 }, { -2, 0 }, { 4, 0 }, { 1, 1 },
        { 3, 3 }, { -2, 2 }, { 5, 2 } };
    const int n = sizeof(points) / sizeof(points[0]);
    point_dist_t answer = { .dist = 0 };

    int result = closest_slow(n, points, &answer);
    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_DOUBLE_EQUAL(answer.dist, 1.414214, tolerance_delta);
    CU_ASSERT(memcmp(&points[0], &answer.p1, sizeof(point_t)) == 0);
    CU_ASSERT(memcmp(&points[3], &answer.p2, sizeof(point_t)) == 0);
}

// point array has null values
// matching points

int closest_slow_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite(
        "closest distance brute force suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        return -1;
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "null does not throw", closest_slow_null)
            || NULL
                == CU_add_test(pSuite, "standard case", closest_slow_standard)
            || NULL
                == CU_add_test(
                       pSuite, "standard case 2", closest_slow_standard2))

    ) {
        return CU_get_error();
    }

    return 0;
}

int main(void)
{
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    int result = euclid_dist_suite();
    if (result != 0) {
        CU_cleanup_registry();
        return result;
    }

    result = closest_distance_suite();
    if (result != 0) {
        CU_cleanup_registry();
        return result;
    }

    result = closest_slow_suite();
    if (result != 0) {
        CU_cleanup_registry();
        return result;
    }

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    /* Clean up registry and return */
    CU_cleanup_registry();
    return CU_get_error();
}
