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

#ifndef __SCL_CORE_H__
#define __SCL_CORE_H__

//SCL_BEGIN_DECLS

#define Uses_SCIM_ATTRIBUTE

#include <string>
#include <scim.h>
#include <sclcommon.h>
#include "sclcoretypes.h"
#include "sclcorecallback.h"

namespace scl
{

/* To use pimpl idiom */
class CSCLCoreImpl;

/**
 * @brief The base class that provides features for a soft-keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 */
class CSCLCore
{
public:
    CSCLCore(ISCLCoreEventCallback *callback);
    ~CSCLCore();

    void run();

    /**
     * @brief Request ISF to reload all configuration.
     */
    void config_reload();

    /**
     * @brief Request ISF to read a integer value with given key name.
     */
    sclboolean config_read_int(const sclchar *name, sclint &value);

    /**
     * @brief Request ISF to read a string value with given key name.
     */
    sclboolean config_read_string(const sclchar *name, std::string &value);

    /**
     * @brief Request ISF to write a integer value with given key name.
     */
    sclboolean config_write_int(const sclchar *name, sclint value);

    /**
     * @brief Request ISF to write a string value with given key name.
     */
    sclboolean config_write_string(const sclchar *name, const std::string value);

    /**
     * @brief Request ISF to erase stored value with given key name.
     */
    sclboolean config_erase(const sclchar *name);

    /**
     * @brief Request ISF to permanently writes all changes.
     */
    sclboolean config_flush(void);

    /**
     * @brief Send an Event to IMEngine
     *
     * @param[in] ic The handle of the IMEngineInstance to receive the event.
     *            -1 means the currently focused IMEngineInstance.
     * @param[in] ic_uuid The UUID of the IMEngineInstance. NULL means don't match.
     * @param[in] command The command to be sent.
     * @param[in] value The value corresponding to the command.
     */
    void send_imengine_event(sclint ic, const sclchar *ic_uuid, const sclint command, const sclu32 value);

    /**
     * @brief Send a reset keyboard Event to IMEngine
     */
    void reset_keyboard_ise();

    /**
     * @brief Send a KeyEvent to ISF
     *
     * @param[in] ic The handle of the IMEngineInstance to receive the event.
     *            -1 means the currently focused IMEngineInstance.
     * @param[in] ic_uuid The UUID of the IMEngineInstance. NULL means don't match.
     * @param[in] keycode The KeyEvent to be sent.
     * @param[in] keymask The modifier key mask
     */
    void send_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask);

    /**
     * @brief Forward a KeyEvent to client application directly.
     *
     * @param[in] ic The handle of the client Input Context to receive the event.
     *            -1 means the currently focused Input Context.
     * @param[in] ic_uuid The UUID of the IMEngineInstance. NULL means don't match.
     * @param[in] keycode The KeyEvent to be sent.
     * @param[in] keymask The modifier key mask
     */
    void forward_key_event(sclint ic, const sclchar *ic_uuid, sclu32 keycode, sclu16 keymask);

    /**
     * @brief Commit a string to client application directly.
     *
     * @param[in] ic The handle of the client Input Context to receive the WideString.
     *            -1 means the currently focused Input Context.
     * @param[in] ic_uuid The UUID of the IMEngine used by the Input Context.
     *            NULL means don't match.
     * @param[in] str The UTF-8 string to be committed.
     */
    void commit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str);

    void select_candidate(int index);
    /**
     * @brief Request to show preedit string.
     *
     * @param[in] ic The handle of the client Input Context to receive the request.
     *            -1 means the currently focused Input Context.
     * @param[in] ic_uuid The UUID of the IMEngine used by the Input Context.
     *            NULL means don't match.
     */
    void show_preedit_string(sclint ic, const sclchar *ic_uuid);

    /**
     * @brief Request to show aux string.
     */
    void show_aux_string(void);

    /**
     * @brief Request to show candidate string.
     */
    void show_candidate_string(void);

    /**
     * @brief Request to show associate string.
     */
    void show_associate_string(void);

    /**
     * @brief Request to hide preedit string.
     *
     * @param[in] ic The handle of the client Input Context to receive the request.
     *            -1 means the currently focused Input Context.
     * @param[in] ic_uuid The UUID of the IMEngine used by the Input Context.
     *            NULL means don't match.
     */
    void hide_preedit_string(sclint ic, const sclchar *ic_uuid);

    /**
     * @brief Request to hide aux string.
     */
    void hide_aux_string(void);

    /**
     * @brief Request to hide candidate string.
     */
    void hide_candidate_string(void);

    /**
     * @brief Request to hide associate string.
     */
    void hide_associate_string(void);

    /**
     * @brief Update a new string for preedit.
     *
     * @param[in] ic The handle of the client Input Context to receive the WideString.
     *            -1 means the currently focused Input Context.
     * @param[in] ic_uuid The UUID of the IMEngine used by the Input Context.
     *            NULL means don't match.
     * @param[in] str The UTF-8 string to be updated.
     */
    void update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str);

    /**
     * @brief Update a new string for preedit.
     *
     * @param[in] ic The handle of the client Input Context to receive the WideString.
     *            -1 means the currently focused Input Context.
     * @param[in] ic_uuid The UUID of the IMEngine used by the Input Context.
     *            NULL means don't match.
     * @param[in] str The UTF-8 string to be updated.
     * @param[in] attrs The attribute list for preedit string.
     */
    void update_preedit_string(sclint ic, const sclchar *ic_uuid, const sclchar *str, const scim::AttributeList &attrs);

    /**
     * @brief Update a new string for aux.
     *
     * @param[in] str The string to be updated.
     * @param[in] attrs The attribute list for aux string.
     */
    //void update_aux_string (const sclchar *str, const AttributeList &attrs) const;
    void update_aux_string(const sclchar *str);

    /**
     * @brief Request to update candidate.
     *
     * @param[in] table The lookup table for candidate.
     */
    void update_candidate_string(const SclCandidateTable &table) const;

    /**
     * @brief Request to update associate.
     *
     * @param[in] table The lookup table for associate.
     */
    void update_associate_string(const SclCandidateTable &table) const;

    /**
     * @ brief When the input context of ISE is changed,
     *         ISE can call this function to notify application
     *
     * @param[in] type  type of event.
     * @param[in] value value of event.
     */
    void update_input_context(sclu32 type, sclu32 value);

    /**
     * @ brief When the geometry of ISE is changed,
     *         ISE can call this function to notify application
     *
     * @param[in] x x position of ISE geometry.
     * @param[in] y y position of ISE geometry.
     * @param[in] width the width of ISE geometry.
     * @param[in] height the height of ISE geometry.
     */
    void update_geometry(sclint x, sclint y, sclint width, sclint height);

    /**
     * @ brief Request to get surrounding text.
     *
     * @param[in] ic_uuid The helper ISE UUID.
     * @param[in] maxlen_before The max length of before.
     * @param[in] maxlen_after The max length of after.
     */
    void get_surrounding_text(const sclchar *ic_uuid, sclint maxlen_before, sclint maxlen_after) const;

    /**
     * @ brief Request to delete surrounding text.
     *
     * @param[in] offset The offset for cursor position.
     * @param[in] len The length for delete text.
     */
    void delete_surrounding_text(sclint offset, sclint len) const;

    /**
     * @ brief Set candidate position in screen.
     *
     * @param[in] left The x position in screen.
     * @param[in] top The y position in screen.
     */
    void set_candidate_position(sclint left, sclint top);

    /**
     * @ brief Set soft candidate style.
     *
     * @param[in] enable The flag for soft candidate style.
     */
    void enable_soft_candidate(sclboolean enable);

    /**
     * @ brief Request to hide candidate window.
     */
    void candidate_hide(void);

    /**
     * @ brief Request to get candidate window size and position.
     *
     * @param[in] uuid The helper ISE UUID.
     */
    //void get_candidate_window_geometry (const String           &uuid) const;

    /**
     * @ brief Set current keyboard ISE.
     *
     * @param[in] uuid The keyboard ISE UUID.
     */
    void set_keyboard_ise_by_uuid(const sclchar *uuid);

    /**
     * @ brief Request to get current keyboard ISE information.
     *
     * @param[in] uuid The helper ISE UUID.
     */
    void get_keyboard_ise(const sclchar *uuid);

    /**
     * @ brief Request to get uuid list of all keyboard ISEs.
     *
     * @param[in] uuid The helper ISE UUID.
     */
    //void get_keyboard_ise_list    (const String                &uuid) const;

    /**
     * @brief This API returns the pointer to main keyboard window
     */
    sclwindow get_main_window();

    /**
     * @brief This API updates the keyboard window's geometry information
     */
    void set_keyboard_size_hints(SclSize portrait, SclSize landscape);

    /**
     * @brief Requests to create an option window
     */
    sclwindow create_option_window();

    /**
     * @brief Requests to destroy an option window
     */
    void destroy_option_window(sclwindow window);

    /**
     * @ brief Request to select text.
     *
     * @param[in] start The start cursor position in text.
     * @param[in] end The end cursor position in text.
     */
    void set_selection(sclint start, sclint end);

    void send_private_command(const sclchar *command);

private:
    CSCLCoreImpl *m_impl;
};

}

//SCL_END_DECLS

#endif //__SCL_CORE_H__
