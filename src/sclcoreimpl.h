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

#ifndef __SCL_CORE_IMPL_H__
#define __SCL_CORE_IMPL_H__

#include "sclcore.h"
#include "sclcoreui.h"
#include "sclcorecallback.h"
#include "sclconnection.h"

//SCL_BEGIN_DECLS

namespace scl
{

class CSCLCoreImpl
{
private:
    CSCLCoreImpl();

    sclchar* m_display;

public:
    ~CSCLCoreImpl();
    static CSCLCoreImpl* get_instance();

    void init(const sclchar *display);
    void fini();

    void on_run(const sclchar *uuid, const sclchar *display);
    void run();

    void set_core_event_callback(ISCLCoreEventCallback *callback);
    ISCLCoreEventCallback* get_core_event_callback();

    CSCLCoreUI* get_core_ui();
    CSCLConnection* get_connection();
    sclchar* get_uuid();

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
    //void update_aux_string (const sclchar *str, const AttributeList &attrs) const;
    void update_aux_string(const sclchar *str);
    //void update_candidate_string (const LookupTable &table) const;
    //void update_associate_string (const LookupTable &table) const;
    void update_input_context(sclu32 type, sclu32 value);
    void update_geometry(sclint x, sclint y, sclint width, sclint height);
    void get_surrounding_text(const sclchar* ic_uuid, sclint maxlen_before, sclint maxlen_after) const;
	void get_selection(const sclchar* ic_uuid) const;
    void delete_surrounding_text(sclint offset, sclint len) const;
    void set_candidate_position(sclint left, sclint top);
    void enable_soft_candidate(sclboolean enable);
    void candidate_hide(void);
    //void get_candidate_window_geometry (const String           &uuid) const;
    void set_keyboard_ise_by_uuid(const sclchar *uuid);
    void get_keyboard_ise(const sclchar *uuid);
    //void get_keyboard_ise_list    (const String                &uuid) const;

    sclwindow get_main_window();
    int get_screen_rotation_degree();
    void set_keyboard_size_hints(SclSize portrait, SclSize landscape);

    sclwindow create_option_window();
    void destroy_option_window(sclwindow window);

    void set_selection(sclint start, sclint end);
    void send_private_command(const sclchar *command);

private:
    ISCLCoreEventCallback *m_event_callback;

    CSCLConnection m_connection;
    CSCLCoreUI m_core_ui;
    sclchar *m_uuid;
};

}

//SCL_END_DECLS

#endif //__SCL_CORE_IMPL_H__
