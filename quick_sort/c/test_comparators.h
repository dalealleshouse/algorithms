typedef struct test_struct {
    int foo;
    int bar;
    char sorter;
} test_struct_t;

typedef int (*comparator)(const void* x, const void* y);

int int_comparator(const void* x, const void* y);

int struct_comparator(const void* x, const void* y);

int pointer_comparator(const void* x, const void* y);
