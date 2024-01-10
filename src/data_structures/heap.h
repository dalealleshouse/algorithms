/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "comparators.h"
#include "result_code.h"

struct Heap_t;
typedef struct Heap_t Heap;

ResultCode Heap_Create(size_t size, sort_strategy, Heap**);
ResultCode Heap_Insert(Heap*, void*);
ResultCode Heap_Resize(Heap*, size_t);
ResultCode Heap_Reproiritize(Heap*, void*);
ResultCode Heap_Delete(Heap*, void*);
ResultCode Heap_Extract(Heap*, void**);
ResultCode Heap_Peek(Heap*, void**);

size_t Heap_Size(Heap*);
size_t Heap_MaxSize(Heap*);
bool Heap_Exists(Heap*, void*);
bool Heap_IsEmpty(Heap*);
void Heap_Destroy(Heap*, freer);
