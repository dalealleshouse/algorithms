#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "closest_pair.h"

void print_error(const char* err)
{
#ifdef _DEBUG
    fprintf(stderr, "%s\n", err);
#else
    (void)err;
#endif
}

void _print_point(const point_t* p) { printf("x=%f, y=%f\n", p->x, p->y); }

int euclid_dist(const point_t* p1, const point_t* p2, double* result)
{
    if (p1 == NULL || p2 == NULL || result == NULL) {
        print_error("null pointer passed to euclid_dist");
        return -1;
    }

    *result = sqrt(pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2));

    if (!isfinite(*result)) {
        print_error("non finite result returned from euclid_dist");
        return -2;
    }

    return 0;
}

int _point_comparator(
    const void* x, const void* y, double (*comparator)(point_t*, point_t*))
{
    if (x == y)
        return 0;

    if (x == NULL)
        return -1;

    if (y == NULL)
        return 1;

    point_t* p1 = (point_t*)x;
    point_t* p2 = (point_t*)y;

    double test_result = comparator(p1, p2);
    if (test_result > 0)
        return 1;

    if (test_result < 0)
        return -1;

    return 0;
}

double _x_comparator(point_t* p1, point_t* p2) { return p1->x - p2->x; }
double _y_comparator(point_t* p1, point_t* p2) { return p1->y - p2->y; }
int _point_x_comparator(const void* x, const void* y)
{
    return _point_comparator(x, y, _x_comparator);
}
int _point_y_comparator(const void* x, const void* y)
{
    return _point_comparator(x, y, _y_comparator);
}

int closest_slow(const int n, const point_t points[n], point_dist_t* result)
{
    if (points == NULL || result == NULL) {
        print_error("null pointer passed to closest_slow");
        return -1;
    }

    if (n <= 1) {
        print_error("when calling closest_slow, n must be greater than 1");
        return -2;
    }

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dist;
            if (euclid_dist(&points[i], &points[j], &dist) != 0)
                return -3;

            if (dist < result->dist || (i == 0 && j == 1)) {
                result->p1 = points[i];
                result->p2 = points[j];
                result->dist = dist;
            }
        }
    }

    return 0;
}

point_dist_t* min(point_dist_t* x, point_dist_t* y)
{
    if (x == NULL || y == NULL)
        return NULL;

    return (x->dist < y->dist) ? x : y;
}

int _closest_split(const int n, const point_t by_x[n], const point_t by_y[n],
    const point_dist_t* delta, point_dist_t* result)
{
    if (by_x == NULL || by_y == NULL || result == NULL) {
        print_error("null pointer passed to _closest_split");
        return -1;
    }

    if (n <= 1) {
        print_error("when calling _closest_split, n must be greater than 1");
        return -2;
    }

    // Find the median x point
    const double x_bar = by_x[n / 2 - 1].x;

    // Find the point that are less than delta away from the x_bar
    const double high_x = x_bar + delta->dist;
    const double low_x = x_bar - delta->dist;
    int length = 0;
    point_t strip[n];
    for (int i = 0; i < n; i++)
        if (by_y[i].x < high_x && by_y[i].x > low_x)
            strip[length++] = by_y[i];

    *result = *delta;
    for (int i = 0; i < length - 1; i++) {
        // This is crazy, but the closest pair in the split set is guaranteed
        // to be no more then 7 places away when the set of points is sorted by
        // y. This inner loop is considered constant time b/c there is a finite
        // number of iterations regardless of the size of the input
        for (int j = i + 1; j < i + 7 && i + j < length; j++) {
            double dist;
            if (euclid_dist(&strip[i], &strip[i + j], &dist) < 0)
                return -3;

            if (dist < result->dist) {
                result->dist = dist;
                result->p1 = strip[i];
                result->p2 = strip[i + j];
            }
        }
    }

    return 0;
}

int _closest_distance(const int n, const point_t by_x[n],
    const point_t by_y[n], point_dist_t* result)
{
    if (by_x == NULL || by_y == NULL || result == NULL) {
        print_error("null pointer passed to _closest_distance");
        return -1;
    }

    if (n <= 1) {
        print_error(
            "when calling _closest_distance, n must be greater than 1");
        return -2;
    }

    // Base case
    if (n <= 3)
        return closest_slow(n, by_x, result);

    // Split the array up into right and left halves
    int left_half = n / 2;
    int right_half = n - left_half;

    // left and right halves sorted by x
    const point_t* left_x = by_x;
    const point_t* right_x = &by_x[left_half];

    // We need left and right halves sorted by y. These halves need to contain
    // the same points as the sorted by x halves. It would be possible to
    // simply sort them again using qsort, but that would raise the
    // computational complexity. In order to achive O(n log(n)) this step must
    // be in linear time - O(n). We use the midpoint of the x value to split
    // the y sorted array into two parts.
    //
    // WARNING: If there are identical x values, this will fail. I'll come back
    // and fix this shortcoming later
    double mid_point = by_x[left_half].x;
    point_t left_y[left_half];
    point_t right_y[right_half];

    int left = 0, right = 0;
    for (int i = 0; i < n; i++)
        if (by_y[i].x < mid_point)
            left_y[left++] = by_y[i];
        else
            right_y[right++] = by_y[i];

    point_dist_t left_closest = { .dist = 0 };
    point_dist_t right_closest = { .dist = 0 };

    int result_r;
    if ((result_r
            = _closest_distance(left_half, left_x, left_y, &left_closest))
        != 0)
        return result_r;

    if ((result_r
            = _closest_distance(right_half, right_x, right_y, &right_closest))
        != 0)
        return result_r;

    point_dist_t l_delta = *min(&left_closest, &right_closest);
    if ((result_r = _closest_split(n, by_x, by_y, &l_delta, result)) != 0)
        return result_r;

    return 0;
}

int closest_distance(
    const int n, const point_t points[n], point_dist_t* result)
{
    if (points == NULL || result == NULL || n < 1)
        return -1;

    size_t p_size = sizeof(points[0]);
    size_t arr_size = p_size * n;

    point_t by_x[n];
    point_t by_y[n];

    memcpy(by_x, points, arr_size);
    memcpy(by_y, points, arr_size);

    qsort(by_x, n, p_size, _point_x_comparator);
    qsort(by_y, n, p_size, _point_y_comparator);

    return _closest_distance(n, by_x, by_y, result);
}
