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

#include <vector>
#include <scim_visibility.h>
#include <scim_types.h>
#include <scim_event.h>
#include <ise_context.h>
#include "scltypes.h"
#include "sclcoretypes.h"

#ifndef __SCL_CORE_CALLBACK_H__
#define __SCL_CORE_CALLBACK_H__

//SCL_BEGIN_DECLS

namespace scl
{

struct ISCLCoreEventCallback {
    virtual void on_init() {}
    virtual void on_run(int argc, char **argv) {}
    virtual void on_exit() {}
    virtual void on_attach_input_context(sclint ic, const sclchar *ic_uuid) {}
    virtual void on_detach_input_context(sclint ic, const sclchar *ic_uuid) {}
    virtual void on_reload_config(sclint ic, const sclchar *ic_uuid) {}
    virtual void on_update_spot_location(sclint ic, const sclchar *ic_uuid, sclint x, sclint y) {}
    virtual void on_update_cursor_position(sclint ic, const sclchar *ic_uuid, sclint cursor_pos) {}
    virtual void on_update_surrounding_text(sclint ic, const sclchar *text, sclint cursor) {}
    virtual void on_focus_out(sclint ic, const sclchar *ic_uuid) {}
    virtual void on_focus_in(sclint ic, const sclchar *ic_uuid) {}
    virtual void on_ise_show(sclint ic, const int degree, Ise_Context &context) {}
    virtual void on_ise_hide(sclint ic, const sclchar *ic_uuid) {}
    virtual void on_get_geometry(sclu32 *pos_x, sclu32 *pos_y, sclu32 *width, sclu32 *height) {}
    virtual void on_set_mode(sclu32 mode) {}
    virtual void on_set_language(sclu32 language) {}
    virtual void on_set_imdata(sclchar *buf, sclu32 len) {}
    virtual void on_get_imdata(sclchar **buf, sclu32 *len) {}
    virtual void on_get_language_locale(sclint ic, sclchar **locale) {}
    virtual void on_set_return_key_type(sclu32 type) {}
    virtual void on_get_return_key_type(sclu32 *type) {}
    virtual void on_set_return_key_disable(sclu32 disabled) {}
    virtual void on_get_return_key_disable(sclu32 *disabled) {}
    virtual void on_set_layout(sclu32 layout) {}
    virtual void on_get_layout(sclu32 *layout) {}
    virtual void on_set_caps_mode(sclu32 mode) {}
    virtual void on_reset_input_context(sclint ic, const sclchar *uuid) {}
    virtual void on_update_candidate_geometry(sclint ic, const sclchar *uuid, sclu32 pos_x, sclu32 pos_y, sclu32 width, sclu32 height) {}
    virtual void on_update_keyboard_ise(sclint ic, const sclchar *uuid, const sclchar *ise_name, const sclchar *ise_uuid) {}
    virtual void on_candidate_more_window_show(sclint ic, const sclchar *uuid) {}
    virtual void on_candidate_more_window_hide(sclint ic, const sclchar *uuid) {}
    virtual void on_select_aux(sclint ic, const sclchar *uuid, sclint index) {}
    virtual void on_select_candidate(sclint ic, const sclchar *uuid, sclint index) {}
    virtual void on_candidate_table_page_up(sclint ic, const sclchar *uuid) {}
    virtual void on_candidate_table_page_down(sclint ic, const sclchar *uuid) {}
    virtual void on_update_lookup_table(SclCandidateTable& table) {}
    virtual void on_update_candidate_table_page_size(sclint ic, const sclchar *uuid, sclint page_size) {}
    virtual void on_select_associate(sclint ic, const sclchar *uuid, sclint index) {}
    virtual void on_associate_table_page_up(sclint ic, const sclchar *uuid) {}
    virtual void on_associate_table_page_down(sclint ic, const sclchar *uuid) {}
    virtual void on_update_associate_table_page_size(sclint ic, const sclchar *uuid, sclint page_size) {}
    virtual void on_process_key_event(scim::KeyEvent &key, sclu32 *ret) {}

    virtual void on_set_display_language(const sclchar *language) {}
    virtual void on_set_rotation_degree(sclint degree) {}
    virtual void on_set_accessibility_state(sclboolean state) {}

    virtual void on_create_option_window(sclwindow window, SCLOptionWindowType type) {}
    virtual void on_destroy_option_window(sclwindow window) {}
    virtual void on_check_option_window_availability(sclboolean *avail) { if (avail) *avail = false;}

    virtual void on_candidate_show(sclint ic, const sclchar *ic_uuid) {}
    virtual void on_candidate_hide(sclint ic, const sclchar *ic_uuid) {}
};

}

//SCL_END_DECLS

#endif //__SCL_CORE_CALLBACK_H__
