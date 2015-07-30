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

#include "sclconnection.h"

#include "sclconnection-isf.h"

using namespace scl;

CSCLConnection::CSCLConnection()
{
    m_backend_identifier = "undefined";
    m_impl = NULL;
}

CSCLConnection::~CSCLConnection()
{
}

std::string CSCLConnection::get_backend_indentifier()
{
    return m_backend_identifier;
}

sclboolean CSCLConnection::init()
{
    sclboolean ret = FALSE;
    if (m_impl == NULL) {
        /* There could be other backend implementations.. */
        m_impl = new CSCLConnectionISF;
    }
    if (m_impl) {
        ret = m_impl->init();
    }
    return ret;
}

void CSCLConnection::fini()
{
    if (m_impl) {
        m_impl->fini();
        delete m_impl;
        m_impl = NULL;
    }
}

void CSCLConnection::open_connection(const sclchar *display)
{
    if (m_impl) {
        m_impl->open_connection(display);
    }
}

void CSCLConnection::close_connection()
{
    if (m_impl) {
        m_impl->close_connection();
    }
}

void CSCLConnection::config_reload()
{
    if (m_impl) {
        m_impl->config_reload();
    }
}

sclboolean CSCLConnection::config_read_int(const sclchar *name, sclint &value)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_read_int(name, value);
    }
    return ret;
}

sclboolean CSCLConnection::config_read_string(const sclchar *name, std::string &value)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_read_string(name, value);
    }
    return ret;
}

sclboolean CSCLConnection::config_write_int(const sclchar *name, sclint value)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_write_int(name, value);
    }
    return ret;
}

sclboolean CSCLConnection::config_write_string(const sclchar *name, const std::string value)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_write_string(name, value);
    }
    return ret;
}

sclboolean CSCLConnection::config_erase(const sclchar *name)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->config_erase(name);
    }
    return ret;
}

void CSCLConnection::send_imengine_event(sclint ic, const sclchar *ic_uuid, const sclint command, const sclu32 value)
{
    if (m_impl) {
        m_impl->send_imengine_event(ic, ic_uuid, command, value);
    }
}

void CSCLConnection::reset_keyboard_ise()
{
    if (m_impl) {
        m_impl->reset_keyboard_ise();
    }
}

void CSCLConnection::send_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask)
{
    if (m_impl) {
        m_impl->send_key_event(ic, ic_uuid, keycode, keymask);
    }
}

void CSCLConnection::forward_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask)
{
    if (m_impl) {
        m_impl->forward_key_event(ic, ic_uuid, keycode, keymask);
    }
}

void CSCLConnection::commit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str)
{
    if (m_impl) {
        m_impl->commit_string(ic, ic_uuid, str);
    }
}

void CSCLConnection::select_candidate(int index)
{
    if (m_impl) {
        m_impl->select_candidate(index);
    }
}

void CSCLConnection::show_preedit_string(sclint ic, const sclchar *ic_uuid)
{
    if (m_impl) {
        m_impl->show_preedit_string(ic, ic_uuid);
    }
}

void CSCLConnection::show_aux_string(void)
{
    if (m_impl) {
        m_impl->show_aux_string();
    }
}

void CSCLConnection::show_candidate_string(void)
{
    if (m_impl) {
        m_impl->show_candidate_string();
    }
}

void CSCLConnection::show_associate_string(void)
{
    if (m_impl) {
        m_impl->show_associate_string();
    }
}

void CSCLConnection::hide_preedit_string(sclint ic, const sclchar *ic_uuid)
{
    if (m_impl) {
        m_impl->hide_preedit_string(ic, ic_uuid);
    }
}

void CSCLConnection::hide_aux_string(void)
{
    if (m_impl) {
        m_impl->hide_aux_string();
    }
}

void CSCLConnection::hide_candidate_string(void)
{
    if (m_impl) {
        m_impl->hide_candidate_string();
    }
}

void CSCLConnection::hide_associate_string(void)
{
    if (m_impl) {
        m_impl->hide_associate_string();
    }
}

void CSCLConnection::update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str)
{
    if (m_impl) {
        m_impl->update_preedit_string(ic, ic_uuid, str);
    }
}

void CSCLConnection::update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str, const scim::AttributeList &attrs)
{
    if (m_impl) {
        m_impl->update_preedit_string(ic, ic_uuid, str, attrs);
    }
}

void CSCLConnection::update_aux_string(const sclchar *str)
{
    if (m_impl) {
        m_impl->update_aux_string(str);
    }
}

void CSCLConnection::update_input_context(sclu32 type, sclu32 value)
{
    if (m_impl) {
        m_impl->update_input_context(type, value);
    }
}

void CSCLConnection::get_surrounding_text(const sclchar *ic_uuid, sclint maxlen_before, sclint maxlen_after) const
{
    if (m_impl) {
         m_impl->get_surrounding_text(ic_uuid, maxlen_before, maxlen_after);
     }
}

void CSCLConnection::delete_surrounding_text(sclint offset, sclint len) const
{
    if (m_impl) {
         m_impl->delete_surrounding_text(offset, len);
     }
}

void CSCLConnection::set_candidate_position(sclint left, sclint top)
{
    if (m_impl) {
        m_impl->set_candidate_position(left, top);
    }
}

void CSCLConnection::candidate_hide(void)
{
    if (m_impl) {
        m_impl->candidate_hide();
    }
}

void CSCLConnection::set_keyboard_ise_by_uuid(const sclchar *uuid)
{
    if (m_impl) {
        m_impl->set_keyboard_ise_by_uuid(uuid);
    }
}

void CSCLConnection::get_keyboard_ise(const sclchar *uuid)
{
    if (m_impl) {
        m_impl->get_keyboard_ise(uuid);
    }
}

void CSCLConnection::set_selection(sclint start, sclint end)
{
    if (m_impl) {
        m_impl->set_selection(start, end);
    }
}

void CSCLConnection::update_geometry(int x, int y, int width, int height)
{
    if (m_impl) {
        m_impl->update_geometry(x, y, width, height);
    }
}

