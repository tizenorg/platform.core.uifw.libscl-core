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

#ifndef __SCL_CORE_UI_EFL_H__
#define __SCL_CORE_UI_EFL_H__

#include "sclcoreui.h"

#include <Ecore.h>

#include "legacy_support/websocket.h"

//SCL_BEGIN_DECLS

namespace scl
{

typedef struct {
    sclwindow window;
} OptionWindowInfo;

class CSCLCoreUIEFL : public CSCLCoreUI
{
public:
    CSCLCoreUIEFL();
    ~CSCLCoreUIEFL();

    virtual sclboolean init();
    virtual void fini();

    virtual void run(const sclchar *display);
    virtual sclwindow get_main_window();
    virtual void set_keyboard_size_hints(SclSize portrait, SclSize landscape);
    virtual int get_screen_rotation_degree();

    virtual sclwindow create_option_window(SCLOptionWindowType type);
    virtual void destroy_option_window(sclwindow window);

    void set_screen_rotation_degree(int degree);
private:
    sclboolean m_initialized;

    sclint m_rotation_degree;
    sclwindow m_main_window;

    OptionWindowInfo m_option_window_info[OPTION_WINDOW_TYPE_MAX];

    /* This websocket agent is for supporting Tizen 2.X legacy web IMEs that uses websocket */
    CWebHelperAgentWebSocket m_websocket;
};

}

//SCL_END_DECLS

#endif //__SCL_CORE_UI_EFL_H__
