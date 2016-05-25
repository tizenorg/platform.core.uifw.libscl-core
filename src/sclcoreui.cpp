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

#include <dlog.h>

#include "sclcoreui.h"

#include "sclcoreui-efl.h"

using namespace scl;

CSCLCoreUI::CSCLCoreUI()
{
    m_backend_identifier = "undefined";
    m_impl = NULL;
}

CSCLCoreUI::~CSCLCoreUI()
{
}

std::string CSCLCoreUI::get_backend_indentifier()
{
    return m_backend_identifier;
}

sclboolean CSCLCoreUI::init()
{
    sclboolean ret = FALSE;
    if (m_impl == NULL) {
        /* There could be other backend implementations.. */
        m_impl = new CSCLCoreUIEFL;
        if (m_impl) {
            ret = m_impl->init();
        }
    }
    return ret;
}

void CSCLCoreUI::fini()
{
    if (m_impl) {
        m_impl->fini();
        delete m_impl;
        m_impl = NULL;
    }
}

void CSCLCoreUI::run(const sclchar *display)
{
    if (m_impl) {
        m_impl->run(display);
    }
}

void CSCLCoreUI::set_keyboard_size_hints(SclSize portrait, SclSize landscape)
{
    if (m_impl) {
        m_impl->set_keyboard_size_hints(portrait, landscape);
    }
}

sclwindow CSCLCoreUI::get_main_window()
{
    sclwindow ret = SCLWINDOW_INVALID;
    if (m_impl) {
        ret = m_impl->get_main_window();
    }
    return ret;
}

int CSCLCoreUI::get_screen_rotation_degree()
{
    int ret = 0;
    if (m_impl) {
        ret = m_impl->get_screen_rotation_degree();
    }
    return ret;
}

sclwindow CSCLCoreUI::create_option_window(SCLOptionWindowType type)
{
    sclwindow ret = SCLWINDOW_INVALID;
    if (m_impl) {
        ret = m_impl->create_option_window(type);
    }
    return ret;
}

void CSCLCoreUI::destroy_option_window(sclwindow window)
{
    if (m_impl) {
        m_impl->destroy_option_window(window);
    }
}

void CSCLCoreUI::process_keyboard_ui_state_change(KEYBOARD_UI_STATE state)
{
    if (m_impl) {
        m_impl->process_keyboard_ui_state_change(state);
    }
}