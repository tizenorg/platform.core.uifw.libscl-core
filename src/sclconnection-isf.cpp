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

#include "sclconnection-isf.h"
#include "sclcoreimpl.h"
#include <Elementary.h>
#include <dlog.h>

using namespace scl;

static scim::ConfigPointer _scim_config (0);

/* Slot functions for calling appropriate callback functions */
static void slot_exit (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_ise_hide(ic, ic_uuid.c_str());
            callback->on_exit();
        }
        if (agent) {
            agent->update_ise_exit();
        }
    }
    elm_exit ();
}

static void slot_attach_input_context (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_attach_input_context(ic, ic_uuid.c_str());
        }
    }
}

static void slot_detach_input_context (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_detach_input_context(ic, ic_uuid.c_str());
        }
    }
}

static void slot_reload_config (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_reload_config(ic, ic_uuid.c_str());
        }
    }
}

static void slot_update_screen (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid, int screen_number) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            //callback->on_update_screen(ic, ic_uuid.c_str(), screen_number);
        }
    }
}

static void slot_update_spot_location (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid, int x, int y) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_update_spot_location(ic, ic_uuid.c_str(), x, y);
        }
    }
}

static void slot_update_cursor_position (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid, int cursor_pos) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_update_cursor_position(ic, ic_uuid.c_str(), cursor_pos);
        }
    }
}

static void slot_update_surrounding_text (const scim::HelperAgent *agent, int ic, const scim::String &text, int cursor) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_update_surrounding_text(ic, text.c_str(), cursor);
        }
    }
}

static void slot_trigger_property (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid, const scim::String &property) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            //callback->on_trigger_property(ic, ic_uuid.c_str(), property.c_str());
        }
    }
}

static void slot_focus_out (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_focus_out(ic, ic_uuid.c_str());
        }
    }
}

static void slot_focus_in (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_focus_in(ic, ic_uuid.c_str());
        }
    }
}

static void slot_ise_show (const scim::HelperAgent *agent, int ic, char *buf, size_t &len) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        /* Make sure the appropriate keyboard ise was selected -> is this really necessary? */
        //impl->set_keyboard_ise_by_uuid(impl->get_keyboard_ise_uuid().c_str());

        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            /* Check if effect is enabled */
            Ise_Context ise_context;
            memset(&ise_context, 0x00, sizeof(ise_context));

            if (len >= sizeof(Ise_Context)) {
                memcpy(&ise_context, buf, sizeof(ise_context));

                if (ise_context.imdata_size > 0) {
                    callback->on_set_imdata(buf + sizeof(ise_context), ise_context.imdata_size);
                }
            } else {
                LOGD("\n-=-= WARNING - buf %p len %d size %d \n", buf, len, sizeof(ise_context));
            }
            callback->on_ise_show(ic, impl->get_screen_rotation_degree(), ise_context);
        }
    }
}

static void slot_ise_hide (const scim::HelperAgent *agent, int ic, const scim::String &ic_uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_ise_hide(ic, ic_uuid.c_str());
        }
    }
}

static void slot_get_geometry (const scim::HelperAgent *agent, struct scim::rectinfo &info) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_get_geometry(&(info.pos_x), &(info.pos_y), &(info.width), &(info.height));
        }
    }
}

static void slot_set_mode (const scim::HelperAgent *agent, scim::uint32 &mode) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_set_mode(mode);
        }
    }
}

static void slot_set_language (const scim::HelperAgent *agent, scim::uint32 &language) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_set_language(language);
        }
    }
}

static void slot_set_imdata (const scim::HelperAgent *agent, char *buf, size_t &len) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            scl32 _len = static_cast<scl32>(reinterpret_cast<size_t>(len) & 0xffffffff);
            callback->on_set_imdata(buf, _len);
        }
    }
}

static void slot_get_imdata (const scim::HelperAgent *, char **buf, size_t &len) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            sclu32 _len = 0;
            callback->on_get_imdata(buf, &_len);
            len = _len;
        }
    }
}

static void slot_get_language_locale (const scim::HelperAgent *, int ic, char **locale) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_get_language_locale(ic, locale);
        }
    }
}

static void slot_set_return_key_type (const scim::HelperAgent *agent, scim::uint32 &type) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_set_return_key_type(type);
        }
    }
}

static void slot_get_return_key_type (const scim::HelperAgent *agent, scim::uint32 &type) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_get_return_key_type(&type);
        }
    }
}

static void slot_set_return_key_disable (const scim::HelperAgent *agent, scim::uint32 &disabled) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_set_return_key_disable(disabled);
        }
    }
}

static void slot_get_return_key_disable (const scim::HelperAgent *agent, scim::uint32 &disabled) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_get_return_key_disable(&disabled);
        }
    }
}

static void slot_set_layout (const scim::HelperAgent *agent, scim::uint32 &layout) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_set_layout(layout);
        }
    }
}

static void slot_get_layout (const scim::HelperAgent *agent, scim::uint32 &layout) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_get_layout(&layout);
        }
    }
}

static void slot_set_caps_mode (const scim::HelperAgent *agent, scim::uint32 &mode) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_set_caps_mode(mode);
        }
    }
}

static void slot_reset_input_context (const scim::HelperAgent *agent, int ic, const scim::String &uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_reset_input_context(ic, uuid.c_str());
        }
    }
}

static void slot_update_candidate_geometry (const scim::HelperAgent *agent, int ic, const scim::String &uuid, const scim::rectinfo &info) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_update_candidate_geometry(ic, uuid.c_str(), info.pos_x, info.pos_y, info.width, info.height);
        }
    }
}
static void slot_update_keyboard_ise (const scim::HelperAgent *agent, int ic, const scim::String &uuid,
                                                  const scim::String &ise_name, const scim::String &ise_uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_update_keyboard_ise(ic, uuid.c_str(), ise_name.c_str(), ise_uuid.c_str());
        }
    }
}

static void slot_candidate_more_window_show (const scim::HelperAgent *agent, int ic, const scim::String &uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_candidate_more_window_show(ic, uuid.c_str());
        }
    }
}

static void slot_candidate_more_window_hide (const scim::HelperAgent *agent, int ic, const scim::String &uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_candidate_more_window_hide(ic, uuid.c_str());
        }
    }
}

static void slot_select_aux (const scim::HelperAgent *agent, int ic, const scim::String &uuid, int index) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_select_aux(ic, uuid.c_str(), index);
        }
    }
}

static void slot_select_candidate (const scim::HelperAgent *agent, int ic, const scim::String &uuid, int index) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_select_candidate(ic, uuid.c_str(), index);
        }
    }
}

static void slot_candidate_table_page_up (const scim::HelperAgent *agent, int ic, const scim::String &uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_candidate_table_page_up(ic, uuid.c_str());
        }
    }
}

static void slot_candidate_table_page_down (const scim::HelperAgent *agent, int ic, const scim::String &uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_candidate_table_page_down(ic, uuid.c_str());
        }
    }
}

static void slot_update_candidate_table_page_size (const scim::HelperAgent *, int ic, const scim::String &uuid, int page_size) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_update_candidate_table_page_size(ic, uuid.c_str(), page_size);
        }
    }
}

static void slot_update_lookup_table (const scim::HelperAgent *, scim::LookupTable &table) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            SclCandidateTable lookup_table;
            lookup_table.page_size = table.get_page_size();
            lookup_table.current_page_start = table.get_current_page_start();
            lookup_table.cursor_pos = table.get_cursor_pos();
            lookup_table.cursor_visible = table.is_cursor_visible();
            lookup_table.page_size_fixed = table.is_page_size_fixed();
            lookup_table.candidate_labels.clear();
            for (int page_index = 0;page_index < lookup_table.page_size;page_index++) {
                scim::WideString label = table.get_candidate_label(page_index);
                lookup_table.candidate_labels.push_back(scim::utf8_wcstombs(label).c_str());
            }

            callback->on_update_lookup_table(lookup_table);

            std::vector<scim::WideString> labels;
            for (unsigned int loop = 0;loop < lookup_table.candidate_labels.size();loop++) {
                labels.push_back(scim::utf8_mbstowcs(lookup_table.candidate_labels.at(loop).c_str()));
            }
            table.set_candidate_labels(labels);
        }
    }
}

static void slot_select_associate (const scim::HelperAgent *agent, int ic, const scim::String &uuid, int index) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_select_associate(ic, uuid.c_str(), index);
        }
    }
}

static void slot_associate_table_page_up (const scim::HelperAgent *agent, int ic, const scim::String &uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_associate_table_page_up(ic, uuid.c_str());
        }
    }
}

static void slot_associate_table_page_down (const scim::HelperAgent *agent, int ic, const scim::String &uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_associate_table_page_down(ic, uuid.c_str());
        }
    }
}

static void slot_update_associate_table_page_size (const scim::HelperAgent *, int ic, const scim::String &uuid, int page_size) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_update_associate_table_page_size(ic, uuid.c_str(), page_size);
        }
    }
}

static void slot_show_ise_option_window (const scim::HelperAgent *agent, int ic, const scim::String &uuid) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        CSCLCoreUI *core_ui = impl->get_core_ui();
        if (core_ui) {
            LOGD("slot_show_ise_option_window() called!!!\n");
            core_ui->create_option_window(OPTION_WINDOW_TYPE_SETTING_APPLICATION);
        }
    }
}

static void slot_check_ise_option_window (const scim::HelperAgent *agent, sclu32 &avail) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_check_option_window_availability(reinterpret_cast<sclboolean *>(&avail));
        }
    }
}

static void slot_process_key_event (const scim::HelperAgent *agent, scim::KeyEvent &key, scim::uint32 &ret) {
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_process_key_event (key, &ret);
        }
    }
}

/* Internal input handler function */
Eina_Bool input_handler (void *data, Ecore_Fd_Handler *fd_handler)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        scim::HelperAgent *agent = static_cast<scim::HelperAgent*>(data);
        if (agent) {
            if (agent->has_pending_event()) {
                if (!(agent->filter_event())) {
                    LOGD("helper_agent.filter_event() failed!!!\n");
                    impl->fini();
                    elm_exit();
                }
            } else {
                LOGD("helper_agent.has_pending_event() failed!!!\n");
                impl->fini();
                elm_exit();
            }
        }
    }

    return ECORE_CALLBACK_RENEW;
}

CSCLConnectionISF::CSCLConnectionISF()
{
    m_initialized = FALSE;
    m_fd_handler = NULL;

    m_backend_identifier = "ISF";
}

CSCLConnectionISF::~CSCLConnectionISF()
{
}

sclboolean CSCLConnectionISF::init()
{
    LOGD ("Enter");

    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        sclchar *uuid = impl->get_uuid();
        if (uuid) {
            m_helper_info.uuid = uuid;
        }
    }

    if (!m_initialized) {
        m_helper_agent.signal_connect_exit (scim::slot (slot_exit));
        m_helper_agent.signal_connect_attach_input_context (scim::slot (slot_attach_input_context));
        m_helper_agent.signal_connect_detach_input_context (scim::slot (slot_detach_input_context));
        m_helper_agent.signal_connect_reload_config (scim::slot (slot_reload_config));
        m_helper_agent.signal_connect_update_screen (scim::slot (slot_update_screen));
        m_helper_agent.signal_connect_update_spot_location (scim::slot (slot_update_spot_location));
        m_helper_agent.signal_connect_update_cursor_position (scim::slot (slot_update_cursor_position));
        m_helper_agent.signal_connect_update_surrounding_text (scim::slot (slot_update_surrounding_text));
        m_helper_agent.signal_connect_trigger_property (scim::slot (slot_trigger_property));
        //m_helper_agent.signal_connect_process_imengine_event (slot (slot_process_imengine_event));
        m_helper_agent.signal_connect_focus_out (scim::slot (slot_focus_out));
        m_helper_agent.signal_connect_focus_in (scim::slot (slot_focus_in));
        m_helper_agent.signal_connect_ise_show (scim::slot (slot_ise_show));
        m_helper_agent.signal_connect_ise_hide (scim::slot (slot_ise_hide));
        m_helper_agent.signal_connect_get_geometry (scim::slot (slot_get_geometry));
        m_helper_agent.signal_connect_set_mode (scim::slot (slot_set_mode));
        m_helper_agent.signal_connect_set_language (scim::slot (slot_set_language));
        m_helper_agent.signal_connect_set_imdata (scim::slot (slot_set_imdata));
        m_helper_agent.signal_connect_get_imdata (scim::slot (slot_get_imdata));
        m_helper_agent.signal_connect_get_language_locale (scim::slot (slot_get_language_locale));
        m_helper_agent.signal_connect_set_return_key_type (scim::slot (slot_set_return_key_type));
        m_helper_agent.signal_connect_get_return_key_type (scim::slot (slot_get_return_key_type));
        m_helper_agent.signal_connect_set_return_key_disable (scim::slot (slot_set_return_key_disable));
        m_helper_agent.signal_connect_get_return_key_disable (scim::slot (slot_get_return_key_disable));
        m_helper_agent.signal_connect_get_layout (scim::slot (slot_get_layout));
        m_helper_agent.signal_connect_set_layout (scim::slot (slot_set_layout));
        m_helper_agent.signal_connect_set_caps_mode (scim::slot (slot_set_caps_mode));
        m_helper_agent.signal_connect_reset_input_context (scim::slot (slot_reset_input_context));
        m_helper_agent.signal_connect_update_candidate_geometry (scim::slot (slot_update_candidate_geometry));
        m_helper_agent.signal_connect_update_keyboard_ise (scim::slot (slot_update_keyboard_ise));
        //m_helper_agent.signal_connect_update_keyboard_ise_list (slot (slot_update_keyboard_ise_list));
        m_helper_agent.signal_connect_candidate_more_window_show (scim::slot (slot_candidate_more_window_show));
        m_helper_agent.signal_connect_candidate_more_window_hide (scim::slot (slot_candidate_more_window_hide));
        m_helper_agent.signal_connect_select_aux (scim::slot (slot_select_aux));
        m_helper_agent.signal_connect_select_candidate (scim::slot (slot_select_candidate));
        m_helper_agent.signal_connect_candidate_table_page_up (scim::slot (slot_candidate_table_page_up));
        m_helper_agent.signal_connect_candidate_table_page_down (scim::slot (slot_candidate_table_page_down));
        m_helper_agent.signal_connect_update_candidate_table_page_size (scim::slot (slot_update_candidate_table_page_size));
        m_helper_agent.signal_connect_update_lookup_table (scim::slot (slot_update_lookup_table));
        m_helper_agent.signal_connect_select_associate (scim::slot (slot_select_associate));
        m_helper_agent.signal_connect_associate_table_page_up (scim::slot (slot_associate_table_page_up));
        m_helper_agent.signal_connect_associate_table_page_down (scim::slot (slot_associate_table_page_down));
        m_helper_agent.signal_connect_update_associate_table_page_size (scim::slot (slot_update_associate_table_page_size));
        m_helper_agent.signal_connect_show_option_window (scim::slot (slot_show_ise_option_window));
        m_helper_agent.signal_connect_check_option_window (scim::slot (slot_check_ise_option_window));
        m_helper_agent.signal_connect_process_key_event (scim::slot (slot_process_key_event));

        m_initialized = TRUE;
    }

    return TRUE;
}

void CSCLConnectionISF::fini()
{
    close_connection();
    m_initialized = FALSE;
}

void CSCLConnectionISF::open_connection(const sclchar *display)
{
    if (m_initialized) {
        m_helper_agent.open_connection(m_helper_info, display);
        int fd = m_helper_agent.get_connection_number();

        if (fd >= 0) {
#ifndef WAYLAND
            Evas_Object *main_window = NULL;
            CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
            if (impl) {
                main_window = NATIVE_WINDOW_CAST(impl->get_main_window());
            }

            Ecore_X_Window xwindow = elm_win_xwindow_get(main_window);
            char xid[255];
            snprintf(xid, 255, "%d", xwindow);
            scim::Property prop(xid, "XID", "", "");
            scim::PropertyList props;
            props.push_back(prop);
            m_helper_agent.register_properties(props);
#endif

            m_fd_handler = ecore_main_fd_handler_add(fd, ECORE_FD_READ, input_handler, &m_helper_agent, NULL, NULL);
        }
    }
}
void CSCLConnectionISF::close_connection()
{
    if (m_initialized) {
        if (m_fd_handler) {
            ecore_main_fd_handler_del(m_fd_handler);
            m_fd_handler = NULL;
        }
        m_helper_agent.update_ise_exit();
        m_helper_agent.close_connection();
    }
}

void CSCLConnectionISF::config_reload()
{
    if (m_initialized) {
        m_helper_agent.reload_config();
    }
}

sclboolean CSCLConnectionISF::config_read_int(const sclchar *name, sclint &value)
{
    sclboolean ret = FALSE;
    if (m_initialized && _scim_config) {
        value = _scim_config->read(name, value);
        ret = TRUE;
    }
    return ret;
}

sclboolean CSCLConnectionISF::config_read_string(const sclchar *name, std::string &value)
{
    sclboolean ret = FALSE;
    if (m_initialized && _scim_config) {
        value = _scim_config->read(name, value);
        ret = TRUE;
    }
    return ret;
}

sclboolean CSCLConnectionISF::config_write_int(const sclchar *name, sclint value)
{
    sclboolean ret = FALSE;
    if (m_initialized && _scim_config) {
        _scim_config->write(name, value);
        ret = TRUE;
    }
    return ret;
}

sclboolean CSCLConnectionISF::config_write_string(const sclchar *name, const std::string value)
{
    sclboolean ret = FALSE;
    if (m_initialized && _scim_config) {
        _scim_config->write(name, value);
        ret = TRUE;
    }
    return ret;
}

sclboolean CSCLConnectionISF::config_erase(const sclchar *name)
{
    sclboolean ret = FALSE;
    if (m_initialized && _scim_config) {
        _scim_config->erase(name);
        ret = TRUE;
    }
    return ret;
}

sclboolean CSCLConnectionISF::config_flush(void)
{
    sclboolean ret = FALSE;
    if (m_initialized && _scim_config) {
        _scim_config->flush();
        ret = TRUE;
    }
    return ret;
}

void CSCLConnectionISF::send_imengine_event(sclint ic, const sclchar *ic_uuid, const sclint command, const sclu32 value)
{
    if (m_initialized) {
        scim::String uuid;
        if (ic_uuid) {
            uuid = scim::String(ic_uuid);
        }
        scim::Transaction trans;
        trans.put_command(command);
        trans.put_data (value);
        m_helper_agent.send_imengine_event(ic, uuid, trans);
    }
}

void CSCLConnectionISF::reset_keyboard_ise()
{
    if (m_initialized) {
        m_helper_agent.reset_keyboard_ise();
    }
}

void CSCLConnectionISF::send_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask)
{
    if (m_initialized) {
        scim::String uuid;
        if (ic_uuid) {
            uuid = scim::String(ic_uuid);
        }
        scim::KeyEvent event;
        event.code = keycode;
        event.mask = keymask;
        m_helper_agent.send_key_event(ic, uuid, event);
    }
}

void CSCLConnectionISF::forward_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask)
{
    if (m_initialized) {
        scim::String uuid;
        if (ic_uuid) {
            uuid = scim::String(ic_uuid);
        }
        scim::KeyEvent event;
        event.code = keycode;
        event.mask = keymask;
        m_helper_agent.forward_key_event(ic, uuid, event);
    }
}

void CSCLConnectionISF::commit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str)
{
    if (m_initialized) {
        scim::String uuid;
        if (ic_uuid) {
            uuid = scim::String(ic_uuid);
        }
        m_helper_agent.commit_string(ic, uuid, scim::utf8_mbstowcs(str));
    }
}

void CSCLConnectionISF::select_candidate(int index)
{
    if (m_initialized) {
        m_helper_agent.select_candidate(index);
    }
}

void CSCLConnectionISF::show_preedit_string(sclint ic, const sclchar *ic_uuid)
{
    if (m_initialized) {
        scim::String uuid;
        if (ic_uuid) {
            uuid = scim::String(ic_uuid);
        }
        m_helper_agent.show_preedit_string(ic, uuid);
    }
}

void CSCLConnectionISF::show_aux_string(void)
{
    if (m_initialized) {
        m_helper_agent.show_aux_string();
    }
}

void CSCLConnectionISF::show_candidate_string(void)
{
    if (m_initialized) {
        m_helper_agent.show_candidate_string();
    }
}

void CSCLConnectionISF::show_associate_string(void)
{
    if (m_initialized) {
        m_helper_agent.show_associate_string();
    }
}

void CSCLConnectionISF::hide_preedit_string(sclint ic, const sclchar *ic_uuid)
{
    if (m_initialized) {
        scim::String uuid;
        if (ic_uuid) {
            uuid = scim::String(ic_uuid);
        }
        m_helper_agent.hide_preedit_string(ic, uuid);
    }
}

void CSCLConnectionISF::hide_aux_string(void)
{
    if (m_initialized) {
        m_helper_agent.hide_aux_string();
    }
}

void CSCLConnectionISF::hide_candidate_string(void)
{
    if (m_initialized) {
        m_helper_agent.hide_candidate_string();
    }
}

void CSCLConnectionISF::hide_associate_string(void)
{
    if (m_initialized) {
        m_helper_agent.hide_associate_string();
    }
}

void CSCLConnectionISF::update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str)
{
    if (m_initialized) {
        scim::AttributeList list;
        scim::String uuid;
        if (ic_uuid) {
            uuid = scim::String(ic_uuid);
        }
        m_helper_agent.update_preedit_string(ic, uuid, scim::utf8_mbstowcs(str), list);

        if (str && strlen(str) > 0) {
            show_preedit_string(ic, ic_uuid);
        } else {
            hide_preedit_string(ic, ic_uuid);
        }
    }
}

void CSCLConnectionISF::update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str, const scim::AttributeList &attrs)
{
    if (m_initialized) {
        scim::String uuid;
        if (ic_uuid) {
            uuid = scim::String(ic_uuid);
        }
        m_helper_agent.update_preedit_string(ic, uuid, scim::utf8_mbstowcs(str), attrs);

        if (str && strlen(str) > 0) {
            show_preedit_string(ic, ic_uuid);
        } else {
            hide_preedit_string(ic, ic_uuid);
        }
    }
}

void CSCLConnectionISF::update_aux_string(const sclchar *str)
{
    if (m_initialized) {
        scim::AttributeList list;
        m_helper_agent.update_aux_string(scim::String(str), list);
    }
}

void CSCLConnectionISF::update_input_context(sclu32 type, sclu32 value)
{
    if (m_initialized) {
        m_helper_agent.update_input_context(type, value);
    }
}

void CSCLConnectionISF::get_surrounding_text(const sclchar *ic_uuid, sclint maxlen_before, sclint maxlen_after) const
{
    if (m_initialized) {
        scim::String uuid;
        if (ic_uuid) {
            uuid = scim::String(ic_uuid);
        }
        m_helper_agent.get_surrounding_text(uuid, maxlen_before, maxlen_after);
    }
}

void CSCLConnectionISF::delete_surrounding_text(sclint offset, sclint len) const
{
    if (m_initialized) {
        m_helper_agent.delete_surrounding_text(offset, len);
    }
}

void CSCLConnectionISF::set_candidate_position(sclint left, sclint top)
{
    if (m_initialized) {
        m_helper_agent.set_candidate_position(left, top);
    }
}

void CSCLConnectionISF::candidate_hide(void)
{
    if (m_initialized) {
        m_helper_agent.candidate_hide();
    }
}

void CSCLConnectionISF::set_keyboard_ise_by_uuid(const sclchar *uuid)
{
    if (m_initialized) {
        m_helper_agent.set_keyboard_ise_by_uuid(uuid);
    }
}

void CSCLConnectionISF::get_keyboard_ise(const sclchar *uuid)
{
    if (m_initialized) {
        m_helper_agent.get_keyboard_ise(uuid);
    }
}

void CSCLConnectionISF::set_selection(sclint start, sclint end)
{
    if (m_initialized) {
       m_helper_agent.set_selection(start, end);
    }
}

void CSCLConnectionISF::update_geometry(int x, int y, int width, int height)
{
    if (m_initialized) {
        m_helper_agent.update_geometry(x, y, width, height);
    }
}

extern "C"
{
    void scim_module_init (void) {
    }

    void scim_module_exit (void) {
    }

    void scim_helper_module_run_helper (const scim::String &uuid, const scim::ConfigPointer &config, const scim::String &display) {
        _scim_config = config;
        CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
        if (impl) {
            impl->on_run(uuid.c_str(), display.c_str());
        }
    }
}

