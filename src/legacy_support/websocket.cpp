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
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <Ecore.h>
#include <Ecore_IMF.h>
#include <dlog.h>

#include "websocket.h"

#include <syslog.h>
#include <signal.h>

#include <libwebsockets.h>

pthread_t g_ws_server_thread = (pthread_t)NULL;
pthread_mutex_t g_ws_server_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t g_ws_query_condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t g_ws_query_mutex = PTHREAD_MUTEX_INITIALIZER;

bool g_ws_server_exit = false;
struct lws_context *g_ws_server_context = NULL;

CWebHelperAgentWebSocket* CWebHelperAgentWebSocket::m_current_instance = NULL;

int force_exit = 0;

enum protocols {
    /* always first */
    PROTOCOL_HTTP = 0,

    PROTOCOL_KEYBOARD,

    /* always last */
    MAX_PROTOCOL_COUNT
};

struct per_session_data__http {
    int fd;
};

static int callback_http(struct lws *wsi,
        enum lws_callback_reasons reason,
        void *user, void *in, size_t len)
{
    return 0;
}

struct per_session_data__keyboard {
    int session_id;
    int valid;
};

static int callback_keyboard(struct lws *wsi,
        enum lws_callback_reasons reason,
        void *user, void *in, size_t len);

static struct lws_protocols protocols[] = {
    {
        "http-only",
        callback_http,
        sizeof(struct per_session_data__http),
        0,
    },
    {
        "keyboard-protocol",
        callback_keyboard,
        sizeof(struct per_session_data__keyboard),
        32,
    },
    { NULL, NULL, 0, 0 }
};

static int callback_keyboard(struct lws *wsi,
        enum lws_callback_reasons reason,
        void *user, void *in, size_t len)
{
    static int last_session_id = 0;
    const int bufsize = 512;
    int n = 0;
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + bufsize +
                          LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    struct per_session_data__keyboard *pss = (struct per_session_data__keyboard *)user;
    CWebHelperAgentWebSocket *agent = CWebHelperAgentWebSocket::get_current_instance();

    switch (reason) {

    case LWS_CALLBACK_ESTABLISHED:
        pss->session_id = ++last_session_id;
        LOGD("LWS_CALLBACK_ESTABLISHED : %p %d", g_ws_server_context, pss->session_id);
        pss->valid = false;
        if (g_ws_server_context) {
            lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
        }
        break;

    case LWS_CALLBACK_CLOSED:
        LOGD("LWS_CALLBACK_CLOSED : %d", pss->session_id);
        break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
        if (agent) {
            /* We allow data tranmission only if this client is guaranteed to be valid */
            if (pss->valid) {
                pthread_mutex_lock(&g_ws_server_mutex);
                std::queue<ISE_MESSAGE>& messages = agent->get_send_message_queue();
                while (messages.size() > 0) {
                    ISE_MESSAGE &message = messages.front();
                    std::string str = CISEMessageSerializer::serialize(message);
                    LOGD("SEND_WEBSOCKET_MESSAGE : %s", str.c_str());
                    n = snprintf((char *)p, bufsize, "%s", str.c_str());
                    /* too small for partial */
                    n = lws_write(wsi, p, n, LWS_WRITE_TEXT);
                    messages.pop();
                }
                pthread_mutex_unlock(&g_ws_server_mutex);

                if (n < 0) {
                    lwsl_err("ERROR %d writing to di socket\n", n);
                    return -1;
                }
            } else {
                LOGD("Rejecting data transmission since client is not valid");
            }
        }
        break;

    case LWS_CALLBACK_RECEIVE:
        if (in) {
            std::string str = (const char *)in;
            ISE_MESSAGE message = CISEMessageSerializer::deserialize(str);

            if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_LOGIN]) == 0) {
                /*
                if (message.values.at(0).compare(CMagicKeyManager::get_magic_key()) == 0) {
                    LOGD("LOGIN successful, validating client");
                    pss->valid = true;
                } else {
                    LOGD("LOGIN failed, invalidating client");
                    pss->valid = false;
                }
                */
                pss->valid = true;
            }

            if (pss->valid) {
                pthread_mutex_lock(&g_ws_server_mutex);
                std::queue<ISE_MESSAGE>& messages = agent->get_recv_message_queue();
                messages.push(message);
                pthread_mutex_unlock(&g_ws_server_mutex);

                const char *recved_message = "recved";
                ecore_pipe_write(agent->get_recv_message_pipe(), recved_message, strlen(recved_message));

                /* If we received reply message, let's send signal to wake up our main thread */
                if (message.type.compare(ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_REPLY]) == 0) {
                    pthread_mutex_lock(&g_ws_query_mutex);
                    pthread_cond_signal(&g_ws_query_condition);
                    pthread_mutex_unlock(&g_ws_query_mutex);
                }
            } else {
                LOGD("Ignoring data received since client is not valid");
            }
        }

        break;
    default:
        break;
    }

    return 0;
}

void *process_ws_server(void *data)
{
    while (!force_exit && !g_ws_server_exit) {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        if (g_ws_server_context) {
            lws_service(g_ws_server_context, 50);
        } else {
            LOGD("WARNING : g_ws_server_context is NULL");
        }
    }
    return NULL;
}

void log_func(int level, const char *line)
{
    if (line) {
        LOGD("LEVEL : %d , %s", level, line);
    }
}

CWebHelperAgentWebSocket::CWebHelperAgentWebSocket()
{
    if (m_current_instance != NULL) {
        LOGD("WARNING : m_current_instance is NOT NULL");
    }
    m_current_instance = this;
    m_recv_message_pipe = NULL;
}

CWebHelperAgentWebSocket::~CWebHelperAgentWebSocket()
{
    if (m_current_instance == this) {
        m_current_instance = NULL;
    }

    if (m_recv_message_pipe) {
        ecore_pipe_del(m_recv_message_pipe);
        m_recv_message_pipe = NULL;
    }
}

static void recv_message_pipe_handler(void *data, void *buffer, unsigned int nbyte)
{
    CWebHelperAgentWebSocket *agent = CWebHelperAgentWebSocket::get_current_instance();
    if (agent) {
        agent->process_recved_messages();
    }
}

bool CWebHelperAgentWebSocket::init()
{
    bool ret = true;

    struct lws_context_creation_info info;

    memset(&info, 0, sizeof info);
    info.port = 7681;

    int debug_level = LLL_DEBUG;
    lws_set_log_level(debug_level, log_func);

    info.iface = NULL;
    info.protocols = protocols;
    info.extensions = NULL;
    info.ssl_cert_filepath = NULL;
    info.ssl_private_key_filepath = NULL;
    info.gid = -1;
    info.uid = -1;
    info.options = 0;

    ecore_init();

    /* The WebSocket server is running on a separate thread, and let the thread send a message
        through this pipe to guarantee thread safety */
    m_recv_message_pipe = ecore_pipe_add(recv_message_pipe_handler, NULL);

    /* Let's retry creating server context for a certain number of times */
    const int max_retry_num = 10;
    int retry_num = 0;

    do {
        g_ws_server_context = lws_create_context(&info);
        LOGD("libwebsocket context : %p", g_ws_server_context);
        sleep(1);
    } while (g_ws_server_context == NULL && retry_num++ < max_retry_num);

    pthread_mutex_init(&g_ws_server_mutex, NULL);

    pthread_mutex_init(&g_ws_query_mutex, NULL);
    pthread_cond_init(&g_ws_query_condition, NULL);

    if (g_ws_server_context) {
        if (pthread_create(&g_ws_server_thread, NULL, &process_ws_server, NULL) != 0) {
            g_ws_server_thread = (pthread_t)NULL;
            ret = false;
        }
    } else {
        ret = false;
    }

    on_init();

    return ret;
}

bool CWebHelperAgentWebSocket::exit()
{
    on_exit();

    g_ws_server_exit = true;

    if (m_recv_message_pipe) {
        ecore_pipe_del(m_recv_message_pipe);
        m_recv_message_pipe = NULL;
    }

    if (g_ws_server_thread) {
        pthread_join(g_ws_server_thread, NULL);
    }

    pthread_cond_destroy(&g_ws_query_condition);
    pthread_mutex_destroy(&g_ws_query_mutex);

    pthread_mutex_destroy(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_context_destroy(g_ws_server_context);
        g_ws_server_context = NULL;
    }

    ecore_shutdown();

    return true;
}

void CWebHelperAgentWebSocket::signal(int sig)
{
    force_exit = 1;
}

template<class T>
std::string to_string(T i)
{
    std::stringstream ss;
    std::string s;
    ss << i;
    s = ss.str();

    return s;
}

void CWebHelperAgentWebSocket::on_init()
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_INIT];

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_exit()
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_EXIT];

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_focus_in(int ic)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_FOCUS_IN];
    message.values.push_back(to_string(ic));

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_focus_out(int ic)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_FOCUS_OUT];
    message.values.push_back(to_string(ic));

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_show(int ic)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_SHOW];
    message.values.push_back(to_string(ic));

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }

    LOGD("put into send message buffer");
}

void CWebHelperAgentWebSocket::on_hide(int ic)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_HIDE];
    message.values.push_back(to_string(ic));

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_set_rotation(int degree)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_SET_ROTATION];
    message.values.push_back(to_string(degree));

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_update_cursor_position(int ic, int cursor_pos)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_UPDATE_CURSOR_POSITION];
    message.values.push_back(to_string(ic));
    message.values.push_back(to_string(cursor_pos));

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_update_surrounding_text(int ic, const char *text, int cursor)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_REPLY];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_UPDATE_SURROUNDING_TEXT];
    message.values.push_back(to_string(cursor));
    message.values.push_back(text);

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_update_selection(int ic, const char *text)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_REPLY];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_UPDATE_SELECTION];
    message.values.push_back(text);

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_set_language(unsigned int language)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_SET_LANGUAGE];

    bool found = false;
    for (unsigned int loop = 0;loop < sizeof(ISE_LANGUAGE_TYPES) / sizeof(ISE_TYPE_VALUE_STRING);loop++) {
        if (language == (unsigned int)ISE_LANGUAGE_TYPES[loop].type_value) {
            message.values.push_back(ISE_LANGUAGE_TYPES[loop].type_string);
            found = true;
        }
    }

    if (found) {
        pthread_mutex_lock(&g_ws_server_mutex);
        m_send_message_queue.push(message);
        pthread_mutex_unlock(&g_ws_server_mutex);

        if (g_ws_server_context) {
            lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
        } else {
            LOGD("WARNING : g_ws_server_context is NULL");
        }
    }
}

void CWebHelperAgentWebSocket::on_set_imdata(char *buf, unsigned int len)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_SET_IMDATA];
    message.values.push_back(buf);

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_get_imdata(char **buf, unsigned int *len)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_QUERY];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_GET_IMDATA];

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }

    wait_for_reply_message();

    std::vector<std::string> values;
    /* Check if we received reply for GET_IMDATA message */
    if (process_recved_messages_until_reply_found(
        ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_GET_IMDATA], values)) {
            if (values.size() > 0 && buf && len) {
                int string_length = values.at(0).length();
                (*buf) = new char[string_length + 1];
                if (*buf) {
                    strncpy(*buf, values.at(0).c_str(), string_length);
                    /* Make sure this is a null-terminated string */
                    *(*buf + string_length) = '\0';
                    *len = string_length;
                }
            }
    } else {
        LOGD("process_recved_messages_until_reply_found returned FALSE");
    }
    /* Now process the rest in the recv buffer */
    process_recved_messages();
}

void CWebHelperAgentWebSocket::on_set_return_key_type(unsigned int type)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_SET_RETURN_KEY_TYPE];

    bool found = false;
    for (unsigned int loop = 0;loop < sizeof(ISE_RETURN_KEY_TYPES) / sizeof(ISE_TYPE_VALUE_STRING);loop++) {
        if (type == (unsigned int)ISE_RETURN_KEY_TYPES[loop].type_value) {
            message.values.push_back(ISE_RETURN_KEY_TYPES[loop].type_string);
            found = true;
        }
    }

    if (found) {
        pthread_mutex_lock(&g_ws_server_mutex);
        m_send_message_queue.push(message);
        pthread_mutex_unlock(&g_ws_server_mutex);

        if (g_ws_server_context) {
            lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
        } else {
            LOGD("WARNING : g_ws_server_context is NULL");
        }
    }
}

void CWebHelperAgentWebSocket::on_get_return_key_type(unsigned int *type)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_QUERY];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_GET_RETURN_KEY_TYPE];

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }

    wait_for_reply_message();

    /* Since we are accessing recved buffer, lock the server mutex again */
    pthread_mutex_lock(&g_ws_server_mutex);
    std::vector<std::string> values;
    /* Check if we received reply for GET_RETURN_KEY_TYPE message */
    if (process_recved_messages_until_reply_found(
        ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_GET_RETURN_KEY_TYPE], values)) {
            if (type) {
                for (unsigned int loop = 0;loop < sizeof(ISE_RETURN_KEY_TYPES) / sizeof(ISE_TYPE_VALUE_STRING);loop++) {
                    if (values.at(0).compare(ISE_RETURN_KEY_TYPES[loop].type_string) == 0) {
                        *type = ISE_RETURN_KEY_TYPES[loop].type_value;
                    }
                }
            }
    }
    /* Now process the rest in the recv buffer */
    process_recved_messages();
    pthread_mutex_unlock(&g_ws_server_mutex);
}

void CWebHelperAgentWebSocket::on_set_return_key_disable(unsigned int disabled)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_SET_RETURN_KEY_DISABLE];

    bool found = false;
    for (unsigned int loop = 0;loop < sizeof(ISE_TRUEFALSE_TYPES) / sizeof(ISE_TYPE_VALUE_STRING);loop++) {
        if (disabled == (unsigned int)ISE_TRUEFALSE_TYPES[loop].type_value) {
            message.values.push_back(ISE_TRUEFALSE_TYPES[loop].type_string);
            found = true;
        }
    }

    if (found) {
        pthread_mutex_lock(&g_ws_server_mutex);
        m_send_message_queue.push(message);
        pthread_mutex_unlock(&g_ws_server_mutex);

        if (g_ws_server_context) {
            lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
        } else {
            LOGD("WARNING : g_ws_server_context is NULL");
        }
    }
}

void CWebHelperAgentWebSocket::on_get_return_key_disable(unsigned int *disabled)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_QUERY];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_GET_RETURN_KEY_DISABLE];

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }

    wait_for_reply_message();

    /* Since we are accessing recved buffer, lock the server mutex again */
    pthread_mutex_lock(&g_ws_server_mutex);
    std::vector<std::string> values;
    /* Check if we received reply for GET_RETURN_KEY_DISABLE message */
    if (process_recved_messages_until_reply_found(
        ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_GET_RETURN_KEY_DISABLE], values)) {
            if (disabled) {
                for (unsigned int loop = 0;loop < sizeof(ISE_TRUEFALSE_TYPES) / sizeof(ISE_TYPE_VALUE_STRING);loop++) {
                    if (values.at(0).compare(ISE_TRUEFALSE_TYPES[loop].type_string) == 0) {
                        *disabled = ISE_TRUEFALSE_TYPES[loop].type_value;
                    }
                }
            }
    }
    /* Now process the rest in the recv buffer */
    process_recved_messages();
    pthread_mutex_unlock(&g_ws_server_mutex);
}

void CWebHelperAgentWebSocket::on_set_layout(unsigned int layout)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_SET_LAYOUT];

    bool found = false;
    for (unsigned int loop = 0;loop < sizeof(ISE_LAYOUT_TYPES) / sizeof(ISE_TYPE_VALUE_STRING);loop++) {
        if (layout == (unsigned int)ISE_LAYOUT_TYPES[loop].type_value) {
            message.values.push_back(ISE_LAYOUT_TYPES[loop].type_string);
            found = true;
        }
    }

    if (found) {
        pthread_mutex_lock(&g_ws_server_mutex);
        m_send_message_queue.push(message);
        pthread_mutex_unlock(&g_ws_server_mutex);

        if (g_ws_server_context) {
            lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
        } else {
            LOGD("WARNING : g_ws_server_context is NULL");
        }
    }
}

void CWebHelperAgentWebSocket::on_get_layout(unsigned int *layout)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_QUERY];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_GET_LAYOUT];

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }

    wait_for_reply_message();

    /* Since we are accessing recved buffer, lock the server mutex again */
    pthread_mutex_lock(&g_ws_server_mutex);
    std::vector<std::string> values;
    /* Check if we received reply for GET_LAYOUT message */
    if (process_recved_messages_until_reply_found(
        ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_GET_LAYOUT], values)) {
            if (layout) {
                for (unsigned int loop = 0;loop < sizeof(ISE_LAYOUT_TYPES) / sizeof(ISE_TYPE_VALUE_STRING);loop++) {
                    if (values.at(0).compare(ISE_LAYOUT_TYPES[loop].type_string) == 0) {
                        *layout = ISE_LAYOUT_TYPES[loop].type_value;
                    }
                }
            }
    }
    /* Now process the rest in the recv buffer */
    process_recved_messages();
    pthread_mutex_unlock(&g_ws_server_mutex);
}

void CWebHelperAgentWebSocket::on_reset_input_context(int ic)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_PLAIN];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_RESET_INPUT_CONTEXT];
    message.values.push_back(to_string(ic));

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }
}

void CWebHelperAgentWebSocket::on_process_key_event(unsigned int code, unsigned int mask, unsigned int layout, unsigned int *ret)
{
    ISE_MESSAGE message;
    message.type = ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_QUERY];
    message.command = ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_PROCESS_KEY_EVENT];
    message.values.push_back(to_string(code));
    message.values.push_back(to_string(mask));
    message.values.push_back(to_string(layout));

    pthread_mutex_lock(&g_ws_server_mutex);
    m_send_message_queue.push(message);
    pthread_mutex_unlock(&g_ws_server_mutex);

    if (g_ws_server_context) {
        lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
    } else {
        LOGD("WARNING : g_ws_server_context is NULL");
    }

    wait_for_reply_message();

    /* Since we are accessing recved buffer, lock the server mutex again */
    pthread_mutex_lock(&g_ws_server_mutex);
    std::vector<std::string> values;
    /* Check if we received reply for PROCESS_KEY_EVENT message */
    if (process_recved_messages_until_reply_found(
        ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_PROCESS_KEY_EVENT], values)) {
            if (ret) {
                for (unsigned int loop = 0;loop < sizeof(ISE_TRUEFALSE_TYPES) / sizeof(ISE_TYPE_VALUE_STRING);loop++) {
                    if (values.at(0).compare(ISE_TRUEFALSE_TYPES[loop].type_string) == 0) {
                        *ret = ISE_TRUEFALSE_TYPES[loop].type_value;
                    }
                }
            }
    }
    /* Now process the rest in the recv buffer */
    process_recved_messages();
    pthread_mutex_unlock(&g_ws_server_mutex);
}

CWebHelperAgentWebSocket* CWebHelperAgentWebSocket::get_current_instance()
{
    return m_current_instance;
}

std::queue<ISE_MESSAGE>& CWebHelperAgentWebSocket::get_send_message_queue()
{
    return m_send_message_queue;
}

std::queue<ISE_MESSAGE>& CWebHelperAgentWebSocket::get_recv_message_queue()
{
    return m_recv_message_queue;
}

Ecore_Pipe* CWebHelperAgentWebSocket::get_recv_message_pipe()
{
    return m_recv_message_pipe;
}

void CWebHelperAgentWebSocket::wait_for_reply_message()
{
    /* Let's wait for at most REPLY_TIMEOUT */
    struct timeval now;
    struct timespec timeout;
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + REPLY_TIMEOUT.tv_sec;
    timeout.tv_nsec = (now.tv_usec + REPLY_TIMEOUT.tv_usec) * 1000;
    pthread_mutex_lock(&g_ws_query_mutex);
    pthread_cond_timedwait(&g_ws_query_condition, &g_ws_query_mutex, &timeout);
    pthread_mutex_unlock(&g_ws_query_mutex);

}

void CWebHelperAgentWebSocket::process_recved_messages()
{
    pthread_mutex_lock(&g_ws_server_mutex);

    while (m_recv_message_queue.size() > 0) {
        ISE_MESSAGE &message = m_recv_message_queue.front();

        handle_recved_message(message);

        m_recv_message_queue.pop();
    }

    pthread_mutex_unlock(&g_ws_server_mutex);
}

bool CWebHelperAgentWebSocket::process_recved_messages_until_reply_found(std::string command, std::vector<std::string> &values)
{
    bool ret = false;

    pthread_mutex_lock(&g_ws_server_mutex);

    while (ret == false && m_recv_message_queue.size() > 0) {
        ISE_MESSAGE &message = m_recv_message_queue.front();

        if (message.command.compare(command) == 0 &&
            message.type.compare(ISE_MESSAGE_TYPE_STRINGS[ISE_MESSAGE_TYPE_REPLY]) == 0) {
            ret = true;
            values = message.values;
        }
        handle_recved_message(message);

        m_recv_message_queue.pop();
    }

    pthread_mutex_unlock(&g_ws_server_mutex);

    return ret;
}

void CWebHelperAgentWebSocket::handle_recved_message(ISE_MESSAGE &message)
{
    if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_LOG]) == 0) {
        std::string str = "";
        for (unsigned int loop = 0;loop < message.values.size();loop++) {
            str += message.values.at(loop).c_str();
            if (loop < message.values.size() - 1) {
                str += " ";
            }
        }
        log(str.c_str());
    }
    if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_COMMIT_STRING]) == 0) {
        send_key_event(0xff6b, 0); // Temporarily reset keyboard engine

        std::string str = "";
        for (unsigned int loop = 0;loop < message.values.size();loop++) {
            str += message.values.at(loop).c_str();
            if (loop < message.values.size() - 1) {
                str += " ";
            }
        }
        commit_string(str.c_str());
    }
    if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_UPDATE_PREEDIT_STRING]) == 0) {
        std::string str = "";
        for (unsigned int loop = 0;loop < message.values.size();loop++) {
            str += message.values.at(loop).c_str();
            if (loop < message.values.size() - 1) {
                str += " ";
            }
        }
        update_preedit_string(str.c_str());
    }
    if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_SEND_KEY_EVENT]) == 0) {
        if (message.values.size() == 1) {
            send_key_event(atoi(message.values.at(0).c_str()), 0);
        }
    }
    if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_SET_KEYBOARD_SIZES]) == 0) {
        LOGD("ISE_MESSAGE_COMMAND_SET_KEYBOARD_SIZES");
        if (message.values.size() == 4) {
            LOGD("ISE_MESSAGE_COMMAND_SET_KEYBOARD_SIZES : %d %d %d %d",
                atoi(message.values.at(0).c_str()), atoi(message.values.at(1).c_str()),
                atoi(message.values.at(2).c_str()), atoi(message.values.at(3).c_str()));
            set_keyboard_sizes(
                atoi(message.values.at(0).c_str()), atoi(message.values.at(1).c_str()),
                atoi(message.values.at(2).c_str()), atoi(message.values.at(3).c_str()));
        }
    }
    if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_SET_SELECTION]) == 0) {
        LOGD("ISE_MESSAGE_COMMAND_SET_SELECTION");
        if (message.values.size() == 2) {
            LOGD("ISE_MESSAGE_COMMAND_SET_SELECTION : %d %d",
                atoi(message.values.at(0).c_str()), atoi(message.values.at(1).c_str()));
            set_selection(
                atoi(message.values.at(0).c_str()), atoi(message.values.at(1).c_str()));
        }
    }
    if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_GET_SELECTION]) == 0) {
        if (message.values.size() == 0) {
            get_selection();
        }
    }
    if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_GET_SURROUNDING_TEXT]) == 0) {
        LOGD("ISE_MESSAGE_COMMAND_GET_SURROUNDING_TEXT");
        if (message.values.size() == 2) {
            LOGD("ISE_MESSAGE_COMMAND_GET_SURROUNDING_TEXT : %d %d",
                atoi(message.values.at(0).c_str()), atoi(message.values.at(1).c_str()));
            get_surrounding_text(
                atoi(message.values.at(0).c_str()), atoi(message.values.at(1).c_str()));
        }
    }
    if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_DELETE_SURROUNDING_TEXT]) == 0) {
        LOGD("ISE_MESSAGE_COMMAND_DELETE_SURROUNDING_TEXT");
        if (message.values.size() == 2) {
            LOGD("ISE_MESSAGE_COMMAND_DELETE_SURROUNDING_TEXT : %d %d",
                atoi(message.values.at(0).c_str()), atoi(message.values.at(1).c_str()));
            delete_surrounding_text(
                atoi(message.values.at(0).c_str()), atoi(message.values.at(1).c_str()));
        }
    }
    if (message.command.compare(ISE_MESSAGE_COMMAND_STRINGS[ISE_MESSAGE_COMMAND_LOGIN]) == 0) {
        if (g_ws_server_context) {
            lws_callback_on_writable_all_protocol(g_ws_server_context, &protocols[PROTOCOL_KEYBOARD]);
        } else {
            LOGD("WARNING : g_ws_server_context is NULL");
        }
    }
}
