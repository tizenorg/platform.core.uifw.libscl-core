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

#ifndef __SCL_CONNECTION_ISF_H__
#define __SCL_CONNECTION_ISF_H__

#define Uses_SCIM_UTILITY
#define Uses_SCIM_OBJECT
#define Uses_SCIM_POINTER
#define Uses_SCIM_EVENT
#define Uses_SCIM_HELPER
#define Uses_SCIM_CONFIG_BASE

#include <Ecore.h>
#include <scim.h>
#include <string>
#include "sclconnection.h"

//SCL_BEGIN_DECLS

namespace scl
{

class CSCLConnectionISF : public CSCLConnection
{
public:
    CSCLConnectionISF();
    ~CSCLConnectionISF();

    virtual sclboolean init();
    virtual void fini();

    virtual void open_connection(const sclchar *display);
    virtual void close_connection();

    void config_reload();
    sclboolean config_read_int(const sclchar *name, sclint &value);
    sclboolean config_read_string(const sclchar *name, std::string &value);
    sclboolean config_write_int(const sclchar *name, sclint value);
    sclboolean config_write_string(const sclchar *name, const std::string value);
    sclboolean config_erase(const sclchar *name);
    sclboolean config_flush(void);

    void send_imengine_event(sclint ic, const sclchar *ic_uuid, const sclint command, const sclu32 value);
    void reset_keyboard_ise();
    void send_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask);
    void forward_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask);
    void commit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str);
    void select_candidate(int index);
    void show_preedit_string(sclint ic, const sclchar *ic_uuid);
    void show_aux_string(void);
    void show_candidate_string(void);
    void show_associate_string(void);
    void hide_preedit_string(sclint ic, const sclchar *ic_uuid);
    void hide_aux_string(void);
    void hide_candidate_string(void);
    void hide_associate_string(void);
    void update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str);
    void update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str, const scim::AttributeList &attrs);
    void update_aux_string(const sclchar *str);
    void update_input_context(sclu32 type, sclu32 value);
    void update_geometry(sclint x, sclint y, sclint width, sclint height);
    void get_surrounding_text(const sclchar *ic_uuid, sclint maxlen_before, sclint maxlen_after) const;
    void delete_surrounding_text(sclint offset, sclint len) const;
    void set_candidate_position(sclint left, sclint top);
    void enable_soft_candidate(sclboolean enable);
    void candidate_hide(void);
    void set_keyboard_ise_by_uuid(const sclchar *uuid);
    void get_keyboard_ise(const sclchar *uuid);
    void set_selection(sclint start, sclint end);
    void send_private_command(const char *command);
    sclint get_surrounding_text(sclint maxlen_before, sclint maxlen_after, sclchar **text, int &cursor);
    void get_selection_text(sclchar **text);
private:
    sclboolean m_initialized;

    scim::HelperInfo m_helper_info;
    scim::HelperAgent m_helper_agent;

    Ecore_Fd_Handler *m_fd_handler;
};

}

//SCL_END_DECLS

#endif //__SCL_CONNECTION_ISF_H__
