/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>

int noop(void);

#define CU_TEST_INFO(test_func) \
  { #test_func, test_func }

typedef struct TestThingy {
  int id;
  size_t n;
  int x;
  int y;
  double z;
} TestThingy;
