/*
 * Copyright (c) 2012 - 2014 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "sclcoreimpl.h"

using namespace scl;

CSCLCoreImpl::CSCLCoreImpl()
{
    m_event_callback = NULL;
}

CSCLCoreImpl::~CSCLCoreImpl()
{
}

CSCLCoreImpl*
CSCLCoreImpl::get_instance()
{
    static CSCLCoreImpl instance;
    return &instance;
}

void CSCLCoreImpl::init(const sclchar *display)
{
    m_connection.init();
    m_core_ui.init();

    m_connection.open_connection(display);

    if (m_event_callback) {
        m_event_callback->on_init();
    }
}

void CSCLCoreImpl::fini()
{
    if (m_event_callback) {
        m_event_callback->on_exit();
    }

    m_connection.close_connection();

    m_core_ui.fini();
    m_connection.fini();
}

void CSCLCoreImpl::set_core_event_callback(ISCLCoreEventCallback *callback)
{
    m_event_callback = callback;
}

ISCLCoreEventCallback* CSCLCoreImpl::get_core_event_callback()
{
    ISCLCoreEventCallback* ret = m_event_callback;
    return ret;
}

CSCLCoreUI* CSCLCoreImpl::get_core_ui()
{
    return &m_core_ui;
}

CSCLConnection* CSCLCoreImpl::get_connection()
{
    return &m_connection;
}

void CSCLCoreImpl::config_reload()
{
    m_connection.config_reload();
}

sclboolean CSCLCoreImpl::config_read_int(const sclchar *name, sclint &value)
{
    return m_connection.config_read_int(name, value);
}

sclboolean CSCLCoreImpl::config_read_string(const sclchar *name, std::string &value)
{
    return m_connection.config_read_string(name, value);
}

sclboolean CSCLCoreImpl::config_write_int(const sclchar *name, sclint value)
{
    return m_connection.config_write_int(name, value);
}

sclboolean CSCLCoreImpl::config_write_string(const sclchar *name, const std::string value)
{
    return m_connection.config_write_string(name, value);
}

sclboolean CSCLCoreImpl::config_erase(const sclchar *name)
{
    return m_connection.config_erase(name);
}

void CSCLCoreImpl::send_imengine_event(sclint ic, const sclchar *ic_uuid, const sclint command, const sclu32 value)
{
    m_connection.send_imengine_event(ic, ic_uuid, command, value);
}

void CSCLCoreImpl::reset_keyboard_ise()
{
    m_connection.reset_keyboard_ise();
}

void CSCLCoreImpl::send_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask)
{
    m_connection.send_key_event(ic, ic_uuid, keycode, keymask);
}

void CSCLCoreImpl::forward_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask)
{
    m_connection.forward_key_event(ic, ic_uuid, keycode, keymask);
}

void CSCLCoreImpl::commit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str)
{
    m_connection.commit_string(ic, ic_uuid, str);
}

void CSCLCoreImpl::select_candidate(int index)
{
    m_connection.select_candidate(index);
}

void CSCLCoreImpl::show_preedit_string(sclint ic, const sclchar *ic_uuid)
{
    m_connection.show_preedit_string(ic, ic_uuid);
}

void CSCLCoreImpl::show_aux_string(void)
{
    m_connection.show_aux_string();
}

void CSCLCoreImpl::show_candidate_string(void)
{
    m_connection.show_candidate_string();
}

void CSCLCoreImpl::show_associate_string(void)
{
    m_connection.show_associate_string();
}

void CSCLCoreImpl::hide_preedit_string(sclint ic, const sclchar *ic_uuid)
{
    m_connection.hide_preedit_string(ic, ic_uuid);
}

void CSCLCoreImpl::hide_aux_string(void)
{
    m_connection.hide_aux_string();
}

void CSCLCoreImpl::hide_candidate_string(void)
{
    m_connection.hide_candidate_string();
}

void CSCLCoreImpl::hide_associate_string(void)
{
    m_connection.hide_associate_string();
}

void CSCLCoreImpl::update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str)
{
    m_connection.update_preedit_string(ic, ic_uuid, str);
}

void CSCLCoreImpl::update_aux_string(const sclchar *str)
{
    m_connection.update_aux_string(str);
}

void CSCLCoreImpl::update_input_context(sclu32 type, sclu32 value)
{
    m_connection.update_input_context(type, value);
}

void CSCLCoreImpl::get_surrounding_text(const sclchar* ic_uuid, sclint maxlen_before, sclint maxlen_after) const
{
    m_connection.get_surrounding_text(ic_uuid, maxlen_before, maxlen_after);
}

void CSCLCoreImpl::delete_surrounding_text(sclint offset, sclint len) const
{
    m_connection.delete_surrounding_text(offset, len);
}

void CSCLCoreImpl::set_candidate_position(sclint left, sclint top)
{
    m_connection.set_candidate_position(left, top);
}

void CSCLCoreImpl::candidate_hide(void)
{
    m_connection.candidate_hide();
}

void CSCLCoreImpl::set_keyboard_ise_by_uuid(const sclchar *uuid)
{
    m_connection.set_keyboard_ise_by_uuid(uuid);
}

void CSCLCoreImpl::get_keyboard_ise(const sclchar *uuid)
{
    m_connection.get_keyboard_ise(uuid);
}

void CSCLCoreImpl::run(const sclchar *display)
{
    m_core_ui.init();
    m_connection.init();

    m_core_ui.run(display);

    m_connection.fini();
    m_core_ui.fini();
}

sclwindow CSCLCoreImpl::get_main_window()
{
    return m_core_ui.get_main_window();
}

int CSCLCoreImpl::get_screen_rotation_degree()
{
    return m_core_ui.get_screen_rotation_degree();
}

void CSCLCoreImpl::set_keyboard_size_hints(SclSize portrait, SclSize landscape)
{
    m_core_ui.set_keyboard_size_hints(portrait, landscape);
}

sclwindow CSCLCoreImpl::create_option_window()
{
    return m_core_ui.create_option_window(OPTION_WINDOW_TYPE_NORMAL);
}

void CSCLCoreImpl::destroy_option_window(sclwindow window)
{
    m_core_ui.destroy_option_window(window);
}
