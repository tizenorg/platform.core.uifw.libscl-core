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

#include "sclcore.h"
#include "sclcoreimpl.h"

using namespace scl;

CSCLCore::CSCLCore(ISCLCoreEventCallback *callback)
{
    m_impl = CSCLCoreImpl::get_instance();

    if (m_impl) {
        m_impl->set_core_event_callback(callback);
    }
}

CSCLCore::~CSCLCore()
{
    m_impl = NULL;
}

void CSCLCore::run()
{
    if (m_impl) {
        m_impl->run();
    }
}

void CSCLCore::config_reload()
{
    if (m_impl) {
        m_impl->config_reload();
    }
}

sclboolean CSCLCore::config_read_int(const sclchar *name, sclint &value)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_read_int(name, value);
    }
    return ret;
}

sclboolean CSCLCore::config_read_string(const sclchar *name, std::string &value)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_read_string(name, value);
    }
    return ret;
}

sclboolean CSCLCore::config_write_int(const sclchar *name, sclint value)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_write_int(name, value);
    }
    return ret;
}

sclboolean CSCLCore::config_write_string(const sclchar *name, const std::string value)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_write_string(name, value);
    }
    return ret;
}

sclboolean CSCLCore::config_erase(const sclchar *name)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_erase(name);
    }
    return ret;
}

sclboolean CSCLCore::config_flush(void)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_flush();
    }
    return ret;
}

void CSCLCore::send_imengine_event(sclint ic, const sclchar *ic_uuid, const sclint command, const sclu32 value)
{
    if (m_impl) {
        m_impl->send_imengine_event(ic, ic_uuid, command, value);
    }
}

void CSCLCore::reset_keyboard_ise()
{
    if (m_impl) {
        m_impl->reset_keyboard_ise();
    }
}

void CSCLCore::send_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask)
{
    if (m_impl) {
        m_impl->send_key_event(ic, ic_uuid, keycode, keymask);
    }
}

void CSCLCore::forward_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask)
{
    if (m_impl) {
        m_impl->forward_key_event(ic, ic_uuid, keycode, keymask);
    }
}

void CSCLCore::commit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str)
{
    if (m_impl) {
        m_impl->commit_string(ic, ic_uuid, str);
    }
}

void CSCLCore::select_candidate(int index)
{
    if (m_impl) {
        m_impl->select_candidate(index);
    }
}

void CSCLCore::show_preedit_string(sclint ic, const sclchar *ic_uuid)
{
    if (m_impl) {
        m_impl->show_preedit_string(ic, ic_uuid);
    }
}

void CSCLCore::show_aux_string(void)
{
    if (m_impl) {
        m_impl->show_aux_string();
    }
}

void CSCLCore::show_candidate_string(void)
{
    if (m_impl) {
        m_impl->show_candidate_string();
    }
}

void CSCLCore::show_associate_string(void)
{
    if (m_impl) {
        m_impl->show_associate_string();
    }
}

void CSCLCore::hide_preedit_string(sclint ic, const sclchar *ic_uuid)
{
    if (m_impl) {
        m_impl->hide_preedit_string(ic, ic_uuid);
    }
}

void CSCLCore::hide_aux_string(void)
{
    if (m_impl) {
        m_impl->hide_aux_string();
    }
}

void CSCLCore::hide_candidate_string(void)
{
    if (m_impl) {
        m_impl->hide_candidate_string();
    }
}

void CSCLCore::hide_associate_string(void)
{
    if (m_impl) {
        m_impl->hide_associate_string();
    }
}

void CSCLCore::update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str)
{
    if (m_impl) {
        m_impl->update_preedit_string(ic, ic_uuid, str);
    }
}

void CSCLCore::update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str, const scim::AttributeList &attrs)
{
    if (m_impl) {
        m_impl->update_preedit_string(ic, ic_uuid, str, attrs);
    }
}

void CSCLCore::update_aux_string(const sclchar *str)
{
    if (m_impl) {
        m_impl->update_aux_string(str);
    }
}

void CSCLCore::update_input_context(sclu32 type, sclu32 value)
{
    if (m_impl) {
        m_impl->update_input_context(type, value);
    }
}

void CSCLCore::update_geometry(sclint x, sclint y, sclint width, sclint height)
{
    if (m_impl) {
        m_impl->update_geometry(x, y, width, height);
    }
}

void CSCLCore::get_surrounding_text(const sclchar *ic_uuid, sclint maxlen_before, sclint maxlen_after) const
{
    if (m_impl) {
        m_impl->get_surrounding_text(ic_uuid, maxlen_before, maxlen_after);
    }
}

void CSCLCore::delete_surrounding_text(sclint offset, sclint len) const
{
    if (m_impl) {
        m_impl->delete_surrounding_text(offset, len);
    }
}

void CSCLCore::set_candidate_position(sclint left, sclint top)
{
    if (m_impl) {
        m_impl->set_candidate_position(left, top);
    }
}

void CSCLCore::enable_soft_candidate(sclboolean enable)
{
    if (m_impl) {
        m_impl->enable_soft_candidate(enable);
    }
}

void CSCLCore::candidate_hide(void)
{
    if (m_impl) {
        m_impl->candidate_hide();
    }
}

void CSCLCore::set_keyboard_ise_by_uuid(const sclchar *uuid)
{
    if (m_impl) {
        m_impl->set_keyboard_ise_by_uuid(uuid);
    }
}

void CSCLCore::get_keyboard_ise(const sclchar *uuid)
{
    if (m_impl) {
        m_impl->get_keyboard_ise(uuid);
    }
}

sclwindow CSCLCore::get_main_window()
{
    sclwindow ret = SCLWINDOW_INVALID;
    if (m_impl) {
        ret = m_impl->get_main_window();
    }
    return ret;
}

void CSCLCore::set_keyboard_size_hints(SclSize portrait, SclSize landscape)
{
    if (m_impl) {
        m_impl->set_keyboard_size_hints(portrait, landscape);
    }
}

sclwindow CSCLCore::create_option_window()
{
    sclwindow ret = SCLWINDOW_INVALID;
    if (m_impl) {
        ret = m_impl->create_option_window();
    }
    return ret;
}

void CSCLCore::destroy_option_window(sclwindow window)
{
    if (m_impl) {
        m_impl->destroy_option_window(window);
    }
}

void CSCLCore::set_selection(sclint start, sclint end)
{
    if (m_impl) {
        m_impl->set_selection(start, end);
    }
}

void CSCLCore::send_private_command(const sclchar *command)
{
    if (m_impl) {
        m_impl->send_private_command(command);
    }
}
