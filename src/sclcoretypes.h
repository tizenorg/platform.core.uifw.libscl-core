/*
 * Copyright (c) 2014 - 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __SCL_CORE_TYPES_H__
#define __SCL_CORE_TYPES_H__

#include <string>
#include <vector>

#include "scltypes.h"

namespace scl
{

typedef enum _SCLKeyMask
{
    KEY_MASK_NULL       = 0,
    KEY_MASK_SHIFT      = (1<<0),
    KEY_MASK_RELEASE    = (1<<15),
} SCLKeyMask;

typedef enum _SCLOptionWindowType
{
    OPTION_WINDOW_TYPE_NORMAL,
    OPTION_WINDOW_TYPE_SETTING_APPLICATION,

    OPTION_WINDOW_TYPE_MAX,
} SCLOptionWindowType;

typedef struct _SclCandidateTable {
    sclint page_size;
    sclint current_page_start;
    sclint cursor_pos;
    sclboolean cursor_visible;
    sclboolean page_size_fixed;

    std::vector<std::string> candidate_labels;
    std::vector<std::string> candidate;
} SclCandidateTable;

}

//SCL_END_DECLS

#endif //__SCL_CORE_TYPES_H__
