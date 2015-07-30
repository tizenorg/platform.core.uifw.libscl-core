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

#ifndef __SCL_CONNECTION_H__
#define __SCL_CONNECTION_H__

#define Uses_SCIM_ATTRIBUTE

#include <string>
#include <scim.h>
#include "scltypes.h"
//SCL_BEGIN_DECLS

namespace scl
{
/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLConnection
{
public:
    CSCLConnection();
    virtual ~CSCLConnection();

    std::string get_backend_indentifier();

    virtual sclboolean init();
    virtual void fini();

    virtual void open_connection(const sclchar *display);
    virtual void close_connection();

    virtual void config_reload();
    virtual sclboolean config_read_int(const sclchar *name, sclint &value);
    virtual sclboolean config_read_string(const sclchar *name, std::string &value);
    virtual sclboolean config_write_int(const sclchar *name, sclint value);
    virtual sclboolean config_write_string(const sclchar *name, const std::string value);
    virtual sclboolean config_erase(const sclchar *name);

    virtual void send_imengine_event(sclint ic, const sclchar *ic_uuid, const sclint command, const sclu32 value);
    virtual void reset_keyboard_ise();
    virtual void send_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask);
    virtual void forward_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask);
    virtual void commit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str);
    virtual void select_candidate(int index);
    virtual void show_preedit_string(sclint ic, const sclchar *ic_uuid);
    virtual void show_aux_string(void);
    virtual void show_candidate_string(void);
    virtual void show_associate_string(void);
    virtual void hide_preedit_string(sclint ic, const sclchar *ic_uuid);
    virtual void hide_aux_string(void);
    virtual void hide_candidate_string(void);
    virtual void hide_associate_string(void);
    virtual void update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str);
    virtual void update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str, const scim::AttributeList &attrs);
    virtual void update_aux_string(const sclchar *str);
    virtual void update_input_context(sclu32 type, sclu32 value);
    virtual void get_surrounding_text(const sclchar *ic_uuid, sclint maxlen_before, sclint maxlen_after) const;
    virtual void delete_surrounding_text(sclint offset, sclint len) const;
    virtual void set_candidate_position(sclint left, sclint top);
    virtual void candidate_hide(void);
    virtual void set_keyboard_ise_by_uuid(const sclchar *uuid);
    virtual void get_keyboard_ise(const sclchar *uuid);
    virtual void set_selection(sclint start, sclint end);
    virtual void update_geometry(int x, int y, int width, int height);

protected:
    std::string m_backend_identifier;

private:
    CSCLConnection *m_impl;
};

}

//SCL_END_DECLS

#endif //__SCL_CONNECTION_H__
