#pragma once

typedef int (*comparator)(const void* x, const void* y);
typedef void (*freer)(void* x);
