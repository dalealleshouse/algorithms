#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "linked_list.h"

#define INT2VOIDP(i) (void*)(uintptr_t)(i)

typedef enum {
    INSERTION = 1,
    RANDOM_ACCESS = 2,
    SEQUENTIAL_ACCESS = 3

} operation;

typedef enum {
    LINKED_LIST = 1,
    DISPERSED_LINKED_LIST = 2,
    ARRAY = 3
} structure;

static void linked_list_insertion(list* list, size_t n)
{
    for (size_t i = 0; i < n; i++)
        list_insert_at(list, INT2VOIDP(rand()), 0);
}

static void dis_linked_list_insertion(list* list, size_t n)
{
    list_insert_at(list, INT2VOIDP(rand()), 0);

    for (size_t i = 1; i < n * 3; i++)
        list_insert_at(list, INT2VOIDP(rand()), rand() % i);

    for (size_t i = 1; i < n * 2; i++)
        list_delete_at(list, rand() % list->size);
}

static void* array_insertion(size_t n)
{
    int* temp;
    int* arr = malloc(sizeof(int));
    *arr = rand();

    for (size_t i = 1; i < n; i++) {
        temp = arr;
        arr = (int*)array_insert(temp, i, sizeof(int));

        free(temp);
        arr[0] = rand();
    }

    return arr;
}

static double insertion_time(structure st, size_t n)
{
    clock_t t;
    switch (st) {
    case LINKED_LIST: {
        list list;
        list_init(&list);

        t = clock();
        linked_list_insertion(&list, n);
        t = clock() - t;

        list_free(&list);
    } break;
    case ARRAY: {
        t = clock();
        void* arr = array_insertion(n);
        t = clock() - t;
        free(arr);
    } break;
    default:
        return -1;
    }

    return t;
}

static double random_access_time(structure st, size_t n)
{
    clock_t t;
    uintptr_t val = 0;

    switch (st) {
    case LINKED_LIST: {
        list list;
        list_init(&list);
        linked_list_insertion(&list, n);

        t = clock();
        for (size_t i = 0; i < n; i++) {
            void* item = list_get_at(&list, rand() % (n - 1));
            val += (uintptr_t)item;
        }
        t = clock() - t;

        list_free(&list);
    } break;
    case ARRAY: {
        int* arr = (int*)malloc(n * sizeof(int));

        for (size_t i = 0; i < n; i++)
            arr[i] = rand();

        t = clock();
        for (size_t i = 0; i < n; i++)
            val += arr[rand() % (n - 1)];
        t = clock() - t;

        free(arr);
    } break;
    default:
        return -1;
    }

    // trick the compiler into not optmizing anything out
    printf("%ld\n", val);

    return t;
}

static double seq_access_time(structure st, size_t n)
{
    clock_t t;
    uintptr_t val = 0;

    switch (st) {
    case LINKED_LIST: {
        list list;
        list_init(&list);
        linked_list_insertion(&list, n);

        t = clock();
        list_item* item = list.head;
        while (item != NULL) {
            val += (uintptr_t)item->payload;
            item = item->next;
        }
        t = clock() - t;

        list_free(&list);
    } break;
    case ARRAY: {
        int arr[n];

        for (size_t i = 0; i < n; i++)
            arr[i] = rand();

        t = clock();
        for (size_t i = 0; i < n; i++)
            val += arr[i];
        t = clock() - t;
    } break;
    case DISPERSED_LINKED_LIST: {
        list list;
        list_init(&list);
        dis_linked_list_insertion(&list, n);

        t = clock();
        list_item* item = list.head;
        while (item != NULL) {
            val += (uintptr_t)item->payload;
            item = item->next;
        }
        t = clock() - t;

        list_free(&list);
    } break;
    default:
        return -1;
    }

    // trick the compiler into not optmizing anything out
    printf("%ld\n", val);

    return t;
}

double op_time(operation op, structure st, size_t n)
{
    double t;

    switch (op) {
    case INSERTION:
        t = insertion_time(st, n);
        break;
    case RANDOM_ACCESS:
        t = random_access_time(st, n);
        break;
    case SEQUENTIAL_ACCESS:
        t = seq_access_time(st, n);
        break;
    default:
        return -1;
    }

    double time = ((double)t) / CLOCKS_PER_SEC;
    return time;
}
