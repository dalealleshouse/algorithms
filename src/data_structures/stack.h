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

#include "result_code.h"

struct Stack_t;
typedef struct Stack_t Stack;

ResultCode Stack_Create(Stack**);
ResultCode Stack_Push(Stack*, void*);
ResultCode Stack_Pop(Stack*, void**);
bool Stack_IsEmpty(Stack*);
void Stack_Destroy(Stack*);
