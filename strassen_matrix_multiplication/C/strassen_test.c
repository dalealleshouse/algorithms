#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "strassen.h"

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* ispower2 ****************/

void ispowerof2_return_false_for_invalid()
{
    CU_ASSERT_FALSE(ispowerof2(3));
    CU_ASSERT_FALSE(ispowerof2(7));
    CU_ASSERT_FALSE(ispowerof2(6));
}

void ispowerof2_return_true_for_valid()
{
    CU_ASSERT_TRUE(ispowerof2(2));
    CU_ASSERT_TRUE(ispowerof2(4));
    CU_ASSERT_TRUE(ispowerof2(8));
}

int ispowerof2_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("ispowerof2", init_suite, clean_suite);
    if (NULL == pSuite) {
        return -1;
    }

    /* add the tests to the suite */
    if ((NULL
                == CU_add_test(pSuite, "non power of 2 returns false",
                       ispowerof2_return_false_for_invalid)
            || NULL
                == CU_add_test(pSuite, "power of 2 return true",
                       ispowerof2_return_true_for_valid))

    ) {
        return CU_get_error();
    }

    return 0;
}

/************* multiply matracies **************/
void compare_matrices(const int rows, const int columns,
    const int expected[rows][columns], const int sut[rows][columns])
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            CU_ASSERT_EQUAL(expected[i][j], sut[i][j]);
        }
    }
}

void null_does_not_throw_test(void)
{
    int result = multiply_square_matrices(1, NULL, NULL, NULL);
    CU_ASSERT_EQUAL(result, -1);
}

void multiply_size_one_matrix(void)
{
    int expected[1][1] = { { 25 } };
    int x[][1] = { { 5 } };
    int y[][1] = { { 5 } };
    int output[1][1];

    int result = multiply_square_matrices(1, x, y, output);

    CU_ASSERT_EQUAL(result, 0);
    compare_matrices(1, 1, expected, output);
}

void multiply_square_matrix(void)
{
    int expected[2][2] = { { 19, 22 }, { 43, 50 } };
    int x[][2] = { { 1, 2 }, { 3, 4 } };
    int y[][2] = { { 5, 6 }, { 7, 8 } };
    int output[2][2];

    int result = multiply_square_matrices(2, x, y, output);

    CU_ASSERT_EQUAL(result, 0);
    compare_matrices(2, 2, expected, output);
}

void multiply_square_matrix_n_equal_8(void)
{
    int expected[2][2] = { { 19, 22 }, { 43, 50 } };
    int x[][2] = { { 1, 2 }, { 3, 4 } };
    int y[][2] = { { 5, 6 }, { 7, 8 } };
    int output[2][2];

    int result = multiply_square_matrices(2, x, y, output);

    CU_ASSERT_EQUAL(result, 0);
    compare_matrices(2, 2, expected, output);
}

void return_error_for_non_power_of_2()
{
    const int n = 8;
    const int expected[8][8] = { { 36, 36, 36, 36, 36, 36, 36, 36 },
        { 72, 72, 72, 72, 72, 72, 72, 72 },
        { 108, 108, 108, 108, 108, 108, 108, 108 },
        { 144, 144, 144, 144, 144, 144, 144, 144 },
        { 180, 180, 180, 180, 180, 180, 180, 180 },
        { 216, 216, 216, 216, 216, 216, 216, 216 },
        { 252, 252, 252, 252, 252, 252, 252, 252 },
        { 288, 288, 288, 288, 288, 288, 288, 288 } };

    int x[n][n];
    int output[n][n];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            x[i][j] = i + 1;
        }
    }

    int result = multiply_square_matrices(n, x, x, output);

    CU_ASSERT_EQUAL(result, 0);
    compare_matrices(2, 2, expected, output);
}

int multiply_square_matrices_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("compare_matrices", init_suite, clean_suite);
    if (NULL == pSuite) {
        return -1;
    }

    /* add the tests to the suite */
    if ((NULL
            == CU_add_test(pSuite, "null input does not cause error",
                   null_does_not_throw_test))
        || (NULL
               == CU_add_test(pSuite, "multply size one matrix",
                      multiply_size_one_matrix))

        || (NULL
               == CU_add_test(
                      pSuite, "multply square matrix", multiply_square_matrix))
        || (NULL
               == CU_add_test(pSuite, "multply square matrix of size 8",
                      multiply_square_matrix_n_equal_8))
        || (NULL
               == CU_add_test(pSuite, "return error for non power of 2",
                      return_error_for_non_power_of_2))) {
        return CU_get_error();
    }

    return 0;
}

int main(void)
{
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    int result = multiply_square_matrices_suite();
    if (result != 0) {
        CU_cleanup_registry();
        return result;
    }

    result = ispowerof2_suite();
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
