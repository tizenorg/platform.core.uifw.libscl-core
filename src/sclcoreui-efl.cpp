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

#include "sclcoreui-efl.h"
#include "sclcoreimpl.h"
#include <isf_control.h>
#include <Elementary.h>
#include <dlog.h>

#include <vconf.h>
#include <vconf-keys.h>

#include <glib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>


using namespace scl;


CSCLCoreUIEFL::CSCLCoreUIEFL()
{
    m_initialized = FALSE;

    m_backend_identifier = "EFL";
}

CSCLCoreUIEFL::~CSCLCoreUIEFL()
{
}

sclboolean CSCLCoreUIEFL::init()
{
    sclboolean ret = FALSE;

    m_initialized = TRUE;
    m_rotation_degree = -1;

    for (int loop = 0;loop < OPTION_WINDOW_TYPE_MAX;loop++) {
        m_option_window_info[loop].window = SCLWINDOW_INVALID;
        m_option_window_info[loop].handler = NULL;
    }

    return ret;
}

void CSCLCoreUIEFL::fini()
{
    m_initialized = FALSE;
}

sclwindow CSCLCoreUIEFL::get_main_window()
{
    if (m_initialized) {
        return m_main_window;
    }
    else
        return NULL;
}

void CSCLCoreUIEFL::set_keyboard_size_hints(SclSize portrait, SclSize landscape)
{
    Evas_Object *main_window = NATIVE_WINDOW_CAST(m_main_window);
    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(main_window),   0, 0, 0, portrait.width, portrait.height);
    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(main_window),  90, 0, 0, landscape.height, landscape.width);
    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(main_window), 180, 0, 0, portrait.width, portrait.height);
    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(main_window), 270, 0, 0, landscape.height, landscape.width);
}

const char * extract_themename_from_theme_file_path(const char *filepath) {
    static char themename[_POSIX_PATH_MAX] = {0};
    memset(themename, 0x00, sizeof(themename));

    if (filepath) {
        /* There could be more than 1 theme filepath, separated by : */
        char pathstr[_POSIX_PATH_MAX] = {0};
        strncpy(pathstr, filepath, _POSIX_PATH_MAX - 1);
        for(int loop = 0;loop < _POSIX_PATH_MAX;loop++) {
            if (pathstr[loop] == ':') {
                /* FIXME : Let's consider the 1st theme filepath only for now */
                pathstr[loop] = '\0';
            }
        }

        if (pathstr[0]) {
            const char *filename = ecore_file_file_get(pathstr);
            if (filename) {
                char *stripname = ecore_file_strip_ext(filename);
                if (stripname) {
                    strncpy(themename, stripname, _POSIX_PATH_MAX - 1);
                    free(stripname);
                }
            }
        }
    }

    return themename;
}

void language_changed_cb(keynode_t *key, void* data)
{
    char clang[_POSIX_PATH_MAX] = {0};
    char *vconf_str = vconf_get_str(VCONFKEY_LANGSET);
    if (vconf_str) {
        snprintf(clang, sizeof(clang), "%s",vconf_str);
        free(vconf_str);
    }
    LOGD("current language is %s\n",clang);

    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_set_display_language(clang);
        }
    }
}

void accessibility_changed_cb(keynode_t *key, void* data)
{
    int vconf_value = 0;
    if (vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, &vconf_value) == 0) {
        LOGD("accessibility state : %d\n", vconf_value);

        CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
        if (impl) {
            ISCLCoreEventCallback *callback = impl->get_core_event_callback();
            if (callback) {
                callback->on_set_accessibility_state(vconf_value);
            }
        }
    }
}

static Eina_Bool _client_message_cb(void *data, int type, void *event)
{
    Ecore_X_Event_Client_Message *ev = (Ecore_X_Event_Client_Message *)event;

    ISCLCoreEventCallback *callback = NULL;
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    Evas_Object *main_window = NULL;
    if (impl) {
        callback = impl->get_core_event_callback();
        main_window = NATIVE_WINDOW_CAST(impl->get_main_window());
    }

#ifndef APPLY_WINDOW_MANAGER_CHANGE
#else
    if (ev->message_type == ECORE_X_ATOM_E_ILLUME_ROTATE_WINDOW_ANGLE) {
        LOGD("ECORE_X_ATOM_E_ILLUME_ROTATE_WINDOW_ANGLE , %d %d\n", ev->data.l[0], gFHiddenState);
        angle = ev->data.l[0];
        ise_set_screen_direction(angle);
        if (!gFHiddenState) {
            ise_show(gLastIC);
        }
    } else if (ev->message_type == ECORE_X_ATOM_E_VIRTUAL_KEYBOARD_STATE) {
        LOGD("ECORE_X_ATOM_E_VIRTUAL_KEYBOARD_STATE , %d\n", ev->data.l[0]);
        elm_win_keyboard_mode_set(main_window, (Elm_Win_Keyboard_Mode)(ev->data.l[0]));
        gFHiddenState = !(ev->data.l[0]);
    }
#endif

    if (ev->message_type == ECORE_X_ATOM_E_WINDOW_ROTATION_CHANGE_REQUEST) {
        if (ev->win == elm_win_xwindow_get(main_window)) {
            int degree = ev->data.l[1];
            CSCLCoreUIEFL *coreui = static_cast<CSCLCoreUIEFL*>(data);
            if (coreui) {
                coreui->set_screen_rotation_degree(degree);
            }
            LOGD("_ECORE_X_ATOM_E_WINDOW_ROTATION_REQUEST, %d\n", degree);
            if (callback) {
                callback->on_set_rotation_degree(degree);
            }
            Ecore_X_Window control_window = 0;
            Ecore_X_Atom atom = ecore_x_atom_get("_ISF_CONTROL_WINDOW");
            Ecore_X_Window root = ecore_x_window_root_first_get();
            if (ecore_x_window_prop_xid_get(root, atom, ECORE_X_ATOM_WINDOW, &control_window, 1) == 1) {
                ecore_x_client_message32_send(control_window, ECORE_X_ATOM_E_WINDOW_ROTATION_CHANGE_REQUEST,
                    ECORE_X_EVENT_MASK_WINDOW_CONFIGURE,
                    ev->data.l[0], ev->data.l[1], ev->data.l[2], ev->data.l[3], ev->data.l[4]);
            }
        }
    }

    return ECORE_CALLBACK_RENEW;
}

int CSCLCoreUIEFL::get_screen_rotation_degree()
{
    int angle = 0;
    if (m_rotation_degree == -1) {
        int  ret = 0;
        Atom type_return;
        int  format_return;
        unsigned long    nitems_return;
        unsigned long    bytes_after_return;
        unsigned char   *data_window = NULL;
        unsigned char   *data_angle = NULL;

        Ecore_X_Window app_window = 0;

        Evas_Object *keypad_win = NATIVE_WINDOW_CAST(m_main_window);

        LOGD("Trying to get app window degree for %p\n", keypad_win);
        Ecore_X_Window win = elm_win_xwindow_get(NATIVE_WINDOW_CAST(keypad_win));
        ret = XGetWindowProperty((Display *)ecore_x_display_get(),
            ecore_x_window_root_get(win),
            ecore_x_atom_get("_ISF_ACTIVE_WINDOW"),
            0, G_MAXLONG, False, XA_WINDOW, &type_return,
            &format_return, &nitems_return, &bytes_after_return,
            &data_window);

        if (ret == Success) {
            if ((type_return == XA_WINDOW) && (format_return == 32) && (data_window)) {
                app_window = *(Window *)data_window;

                ret = XGetWindowProperty((Display *)ecore_x_display_get(), app_window,
                    ecore_x_atom_get("_E_ILLUME_ROTATE_WINDOW_ANGLE"),
                    0, G_MAXLONG, False, XA_CARDINAL, &type_return,
                    &format_return, &nitems_return, &bytes_after_return,
                    &data_angle);

                LOGD("app_window : %p, ret %d, %d, %p\n", app_window, ret, type_return, data_angle);
                if (ret == Success) {
                    if (data_angle) {
                        if (type_return == XA_CARDINAL) {
                            angle = *(unsigned int*)data_angle;
                            LOGD("current rotation angle is %p %d\n", app_window, angle);
                        }
                        XFree(data_angle);
                    }
                }
            }
            if (data_window)
                XFree(data_window);
        }
    } else {
        angle = m_rotation_degree;
    }

    return angle;
}

void signal_handler(int sig) {
    elm_exit();
}

void CSCLCoreUIEFL::run(const sclchar *display)
{
    char *argv[4];
    int argc = 3;

    std::string name;
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        sclchar *uuid = impl->get_uuid();
        if (uuid) {
            sclchar *label = NULL;
            if (isf_control_get_ise_info(uuid, &label, NULL, NULL, NULL) == 0) {
                name = std::string(label);
                if (label)
                    free(label);
            }
        }

        argv [0] = const_cast<char *> (name.c_str());
        argv [1] = (char *)"--display";
        argv [2] = const_cast<char *> (display);
        argv [3] = 0;

        elm_init(argc, argv);

        elm_policy_set(ELM_POLICY_THROTTLE, ELM_POLICY_THROTTLE_NEVER);

        Evas_Object *main_window = elm_win_add(NULL, name.c_str(), ELM_WIN_UTILITY);
        m_main_window = SCL_WINDOW_CAST(main_window);

        elm_win_borderless_set(main_window, EINA_TRUE);
        elm_win_keyboard_win_set(main_window, EINA_TRUE);
        elm_win_autodel_set(main_window, EINA_TRUE);
        elm_win_title_set(main_window, name.c_str());
        elm_win_prop_focus_skip_set(main_window, EINA_TRUE);

        unsigned int set = 1;
        ecore_x_window_prop_card32_set(elm_win_xwindow_get(main_window),
            ECORE_X_ATOM_E_WINDOW_ROTATION_SUPPORTED,
            &set, 1);

        ecore_x_icccm_name_class_set(elm_win_xwindow_get(main_window), "Virtual Keyboard", "ISF");

        vconf_notify_key_changed(VCONFKEY_LANGSET, language_changed_cb, NULL);
        vconf_notify_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, accessibility_changed_cb, NULL);

        /* Should we call these callback functions here? */
        language_changed_cb(NULL, NULL);
        accessibility_changed_cb(NULL, NULL);

        impl->init(display);

        Ecore_Event_Handler *XClientMsgHandler =
            ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, _client_message_cb, this);

        signal(SIGQUIT, signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGINT,  signal_handler);
        signal(SIGHUP,  signal_handler);

        elm_run();

        impl->fini();

        vconf_ignore_key_changed(VCONFKEY_LANGSET, language_changed_cb);
        vconf_ignore_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, accessibility_changed_cb);

        if (XClientMsgHandler) {
            ecore_event_handler_del(XClientMsgHandler);
            XClientMsgHandler = NULL;
        }

        elm_shutdown();
    }
}

static Eina_Bool focus_out_cb(void *data, int type, void *event)
{
    OptionWindowInfo *info = static_cast<OptionWindowInfo*>(data);
    if (info) {
        if (info->window) {
            CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
            if (impl) {
                ISCLCoreEventCallback *callback = impl->get_core_event_callback();
                if (callback) {
                    callback->on_destroy_option_window(info->window);
                }
            }

            evas_object_hide(NATIVE_WINDOW_CAST(info->window));
            evas_object_del(NATIVE_WINDOW_CAST(info->window));
            info->window = NULL;
        }
        if (info->handler) {
            ecore_event_handler_del(info->handler);
            info->handler = NULL;
        }
    }

    return ECORE_CALLBACK_CANCEL;
}

static void
set_transient_for_app_window(Evas_Object *window)
{
    /* Set a transient window for window stack */
    /* Gets the current XID of the active window into the root window property  */
    Atom type_return;
    unsigned long nitems_return;
    unsigned long bytes_after_return;
    int format_return;
    unsigned char *data = NULL;
    Ecore_X_Window xAppWindow;
    Ecore_X_Window xWindow = elm_win_xwindow_get(window);
    gint ret = 0;

    ret = XGetWindowProperty((Display *)ecore_x_display_get(), ecore_x_window_root_get(xWindow),
        ecore_x_atom_get("_ISF_ACTIVE_WINDOW"),
        0, G_MAXLONG, False, XA_WINDOW, &type_return,
        &format_return, &nitems_return, &bytes_after_return,
        &data);

    if (ret == Success) {
        if (data) {
            if (type_return == XA_WINDOW) {
                xAppWindow = *(Window *)data;
                LOGD("TRANSIENT_FOR SET : %x , %x", xAppWindow, xWindow);
                ecore_x_icccm_transient_for_set(xWindow, xAppWindow);
            }
            XFree(data);
        }
    }
}

static void
set_transient_for_isf_setting_window(Evas_Object *window)
{
    /* Set a transient window for window stack */
    /* Gets the current XID of the active window into the root window property  */
    Atom type_return;
    unsigned long nitems_return;
    unsigned long bytes_after_return;
    int format_return;
    unsigned char *data = NULL;
    Ecore_X_Window xControlWindow, xSettingWindow;
    Ecore_X_Window xWindow = elm_win_xwindow_get(window);
    gint ret = 0;

    ret = XGetWindowProperty((Display *)ecore_x_display_get(), ecore_x_window_root_get(xWindow),
        ecore_x_atom_get("_ISF_CONTROL_WINDOW"),
        0, G_MAXLONG, False, XA_WINDOW, &type_return,
        &format_return, &nitems_return, &bytes_after_return,
        &data);

    if (ret == Success) {
        if (data) {
            if (type_return == XA_WINDOW) {
                xControlWindow = *(Window *)data;

                ecore_x_window_prop_xid_get(xControlWindow, ecore_x_atom_get("ISF Setting window"),
                    ECORE_X_ATOM_WINDOW, &xSettingWindow, 1);

                LOGD("TRANSIENT_FOR SET : %x , %x", xSettingWindow, xWindow);
                ecore_x_icccm_transient_for_set(xWindow, xSettingWindow);
            }
            XFree(data);
        }
    }
}

sclwindow CSCLCoreUIEFL::create_option_window(SCLOptionWindowType type)
{
    if (type < 0 || type >= OPTION_WINDOW_TYPE_MAX) {
        return SCLWINDOW_INVALID;
    }

    ISCLCoreEventCallback *callback = NULL;
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        callback = impl->get_core_event_callback();
        if (callback) {
            sclboolean ret = false;
            callback->on_check_option_window_availability(&ret);
            if (ret == false) {
                LOGW("on_create_option_window() is not available");
                return SCLWINDOW_INVALID;
            }
        }
        else
            return SCLWINDOW_INVALID;
    }

    /* Just in case the previous option window for setting application exists */
    if (type == OPTION_WINDOW_TYPE_SETTING_APPLICATION) {
        if (m_option_window_info[type].window != SCLWINDOW_INVALID) {
            destroy_option_window(m_option_window_info[type].window);
        }
    }

    Evas_Object *window = elm_win_util_standard_add("Option window", "Option window");

    const char *szProfile[] = {"mobile", ""};
    elm_win_profiles_set(window, szProfile, 1);

    elm_win_borderless_set(window, EINA_TRUE);

    Evas_Coord win_w = 0, win_h = 0;
    ecore_x_window_size_get(ecore_x_window_root_first_get(), &win_w, &win_h);
    int degree = get_screen_rotation_degree();
    if(degree == 90 || degree == 270){
        evas_object_resize(window, win_h, win_w);
    }else{
        evas_object_resize(window, win_w, win_h);
    }

    int rots[] = { 0, 90, 180, 270 };
    elm_win_wm_rotation_available_rotations_set(window, rots, (sizeof(rots) / sizeof(int)));

    elm_win_indicator_mode_set(window, ELM_WIN_INDICATOR_SHOW);

    if (callback) {
        callback->on_create_option_window(window, type);
    }

    Ecore_Event_Handler *handler = NULL;
    if (type == OPTION_WINDOW_TYPE_NORMAL) {
        handler = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_OUT, focus_out_cb, &m_option_window_info[type]);
        set_transient_for_app_window(window);
    } else if (type == OPTION_WINDOW_TYPE_SETTING_APPLICATION) {
        set_transient_for_isf_setting_window(window);
        evas_object_show(window);
    }

    m_option_window_info[type].window = window;
    m_option_window_info[type].handler = handler;

    return window;
}

void CSCLCoreUIEFL::destroy_option_window(sclwindow window)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        ISCLCoreEventCallback *callback = impl->get_core_event_callback();
        if (callback) {
            callback->on_destroy_option_window(window);
        }
    }

    for (int loop = 0;loop < OPTION_WINDOW_TYPE_MAX;loop++) {
        if (m_option_window_info[loop].window == window) {
            evas_object_del(NATIVE_WINDOW_CAST(window));
            m_option_window_info[loop].window = SCLWINDOW_INVALID;
            if (m_option_window_info[loop].handler) {
                ecore_event_handler_del(m_option_window_info[loop].handler);
                m_option_window_info[loop].handler = NULL;
            }
        }
    }
}

void CSCLCoreUIEFL::set_screen_rotation_degree(int degree)
{
    m_rotation_degree = degree;
}
