/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stdbool.h>

#include "result_code.h"

struct Queue_t;
typedef struct Queue_t Queue;

ResultCode Queue_Create(Queue**);
ResultCode Queue_Enqueue(Queue*, void*);
ResultCode Queue_Dequeue(Queue*, void**);
bool Queue_IsEmpty(Queue*);
void Queue_Destroy(Queue*);
