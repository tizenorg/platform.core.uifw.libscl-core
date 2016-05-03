/*
 * Copyright (c) 2012 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <stdio.h>
#include <dlog.h>

#include "web_helper_agent.h"

#include "websocket.h"
#include "sclcoreimpl.h"

using namespace scl;

typedef struct {
    int major_version;
    WEB_HELPER_AGENT_TYPE agent_type;
} WEB_HELPER_AGENT_TYPES_FOR_VERSIONS;

static WEB_HELPER_AGENT_TYPES_FOR_VERSIONS web_helper_agent_types_for_versions[] = {
    {1, WEB_HELPER_AGENT_WEBSOCKET}, /* Major version 1 indicates that it uses websocket for communication */
};

static int WEB_HELPER_AGENT_TYPES_FOR_VERSIONS_NUM = \
    sizeof(web_helper_agent_types_for_versions) / sizeof(WEB_HELPER_AGENT_TYPES_FOR_VERSIONS);

WEB_HELPER_AGENT_TYPE CWebHelperAgent::get_web_helper_agent_type_from_major_version(int version)
{
    for (int loop = 0;loop < WEB_HELPER_AGENT_TYPES_FOR_VERSIONS_NUM;loop++) {
        if (web_helper_agent_types_for_versions[loop].major_version == version) {
            return web_helper_agent_types_for_versions[loop].agent_type;
        }
    }
    return WEB_HELPER_AGENT_UNKNOWN;
}

CWebHelperAgent* CWebHelperAgent::create_web_helper_agent(WEB_HELPER_AGENT_TYPE type)
{
    CWebHelperAgent *ret = NULL;
    if (type == WEB_HELPER_AGENT_WEBSOCKET) {
        ret = new CWebHelperAgentWebSocket;
    }
    return ret;
}

void CWebHelperAgent::destroy_web_helper_agent(CWebHelperAgent* agent)
{
    if (agent) delete agent;
}

CWebHelperAgent::CWebHelperAgent()
{
}

CWebHelperAgent::~CWebHelperAgent()
{

}

bool CWebHelperAgent::init()
{
    return true;
}

bool CWebHelperAgent::exit()
{
    return true;
}

void CWebHelperAgent::signal(int sig)
{

}

void CWebHelperAgent::log(const char *str)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        //impl->logise_log(str);
    }
}

void CWebHelperAgent::commit_string(const char *str)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        impl->commit_string(-1, "", str);
    }
}

void CWebHelperAgent::update_preedit_string(const char *str)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        impl->update_preedit_string(-1, "", str);
    }
}

void CWebHelperAgent::send_key_event(unsigned int key, unsigned int key_mask)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        impl->send_key_event(-1, "", key, key_mask);
    }
}

void CWebHelperAgent::forward_key_event(unsigned int key)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        impl->forward_key_event(-1, "", key, scim::SCIM_KEY_NullMask);
    }
}

void CWebHelperAgent::set_keyboard_sizes(int portrait_width, int portrait_height, int landscape_width, int landscape_height)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        SclSize portrait, landscape;
        portrait.width = portrait_width;
        portrait.height = portrait_height;
        landscape.width = landscape_width;
        landscape.height = landscape_height;
        impl->set_keyboard_size_hints(portrait, landscape);
    }
}

void CWebHelperAgent::set_selection(int start_index, int end_index)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        impl->set_selection(start_index, end_index);
    }
}

void CWebHelperAgent::get_selection()
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        //impl->get_selection(-1, "", );
    }
}

void CWebHelperAgent::get_surrounding_text(int maxlen_before, int maxlen_after)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        impl->get_surrounding_text("", maxlen_before, maxlen_after);
    }
}

void CWebHelperAgent::delete_surrounding_text(int offset, int len)
{
    CSCLCoreImpl *impl = CSCLCoreImpl::get_instance();
    if (impl) {
        impl->delete_surrounding_text(offset, len);
    }
}

std::string CMagicKeyManager::get_magic_key()
{
    static std::string current_magic_key;

    /* If we don't have magic key generated yet */
    if (current_magic_key.length() != MAGIC_KEY_LENGTH) {
        char magic_key[MAGIC_KEY_LENGTH + 1];
        /* We are going to generate a magic key that contains ascii characters in the range of '0' to 'z' */
        const char magic_key_range_lower = '0';
        const char magic_key_range_upper = 'Z';

        srand(time(NULL));
        for(int loop = 0;loop < MAGIC_KEY_LENGTH;loop++) {
            magic_key[loop] = (rand() % (magic_key_range_upper - magic_key_range_lower)) + magic_key_range_lower;
        }
        magic_key[MAGIC_KEY_LENGTH] = '\0';

        current_magic_key = magic_key;
    }

    return current_magic_key;
}
