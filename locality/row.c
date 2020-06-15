// Copyright 2020 Dale Alleshouse
#include <stdint.h>
#include <stdio.h>

#define SIZE 10000

int64_t array[SIZE][SIZE] = {0};

int main() {
  printf("\n Enumerating a %d X %d array in row order", SIZE, SIZE);

  for (size_t i = 0; i < SIZE; i++) {
    for (size_t j = 0; j < SIZE; j++) array[i][j]++;
  }
  return 0;
}
