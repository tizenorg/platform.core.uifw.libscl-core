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

#ifndef _WEB_HELPER_AGENT_H_
#define _WEB_HELPER_AGENT_H_

#define MAGIC_KEY_LENGTH 32
#define VERSION_DELIMITER '.'
#define VERSION_TOKEN_NUM 2 // We are expecting 2 version tokens : MAJOR and MINOR

#include <string>
#include <sstream>
#include <vector>

class CStringTokenizer
{
public:
    static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        unsigned int loop = 0;
        unsigned int start = 0;
        for (loop = 0;loop < s.length();loop++) {
            if (s.at(loop) == delim) {
                std::string item = s.substr(start, loop - start);
                elems.push_back(item);
                start = loop + 1;
            }
        }
        std::string item = s.substr(start, loop - start);
        elems.push_back(item);
        return elems;
    }

    static std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }
};

typedef enum {
    WEB_HELPER_AGENT_UNKNOWN,
    WEB_HELPER_AGENT_WEBSOCKET,
    WEB_HELPER_AGENT_DIRECT,
} WEB_HELPER_AGENT_TYPE;

class CMagicKeyManager
{
public:
    static std::string get_magic_key();
};

class CWebHelperAgent
{
public:
    CWebHelperAgent();
    virtual ~CWebHelperAgent();

    virtual bool init();
    virtual bool exit();

    virtual void signal(int sig);

    virtual void on_init() {}
    virtual void on_exit() {}

    virtual void on_focus_in(int ic) {}
    virtual void on_focus_out(int ic) {}

    virtual void on_show(int ic) {}
    virtual void on_hide(int ic) {}

    virtual void on_set_rotation(int degree) {}

    virtual void on_update_cursor_position(int ic, int cursor_pos) {}
    virtual void on_update_surrounding_text(int ic, const char *text, int cursor) {}
    virtual void on_update_selection(int ic, const char *text) {}

    virtual void on_set_language(unsigned int language) {}

    virtual void on_set_imdata(char *buf, unsigned int len) {}
    virtual void on_get_imdata(char **buf, unsigned int *len) {}

    virtual void on_set_return_key_type(unsigned int type) {}
    virtual void on_get_return_key_type(unsigned int *type) {}

    virtual void on_set_return_key_disable(unsigned int disabled) {}
    virtual void on_get_return_key_disable(unsigned int *disabled) {}

    virtual void on_set_layout(unsigned int layout) {}
    virtual void on_get_layout(unsigned int *layout) {}

    virtual void on_reset_input_context(int ic) {}

    virtual void on_process_key_event(unsigned int code, unsigned int mask, unsigned int layout, unsigned int *ret) {}


    virtual void log(const char *str);
    virtual void commit_string(const char *str);
    virtual void update_preedit_string(const char *str);
    virtual void send_key_event(unsigned int key, unsigned int key_mask);
    virtual void forward_key_event(unsigned int key);
    virtual void set_keyboard_sizes(int portrait_width, int portrait_height, int landscape_width, int landscape_height);
    virtual void set_selection(int start_index, int end_index);
    virtual void get_selection();
    virtual void get_surrounding_text(int maxlen_before, int maxlen_after);
    virtual void delete_surrounding_text(int offset, int len);

public:
    static WEB_HELPER_AGENT_TYPE get_web_helper_agent_type_from_major_version(int version);
    static CWebHelperAgent* create_web_helper_agent(WEB_HELPER_AGENT_TYPE type);
    static void destroy_web_helper_agent(CWebHelperAgent* agent);
};

#endif // _WEB_HELPER_AGENT_H_