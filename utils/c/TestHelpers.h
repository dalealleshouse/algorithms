#pragma once

#include <stddef.h>

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

typedef struct TestThingy {
    int id;
    size_t n;
    int x;
    int y;
    double z;
} TestThingy;

static int noop(void) { return 0; }

void TestFaliedMalloc(void (*tester)(void));
