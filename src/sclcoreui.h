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

#ifndef __SCL_CORE_UI_H__
#define __SCL_CORE_UI_H__

//SCL_BEGIN_DECLS
#include <string>

#include "scltypes.h"
#include "sclcorecallback.h"

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API


namespace scl
{

/**
 * @brief The base class that provides features for a soft-keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 */
class LIBSCL_EXPORT_API CSCLCoreUI
{
public:
    CSCLCoreUI();
    virtual ~CSCLCoreUI();

    std::string get_backend_indentifier();

    virtual sclboolean init();
    virtual void fini();

    virtual void run(const sclchar *display);

    /**
     * @brief This API returns the pointer to main keyboard window
     */
    virtual sclwindow get_main_window();

    /**
     * @brief This API returns the rotation angle of the current screen
     */
    virtual int get_screen_rotation_degree();

    /**
     * @brief This API updates the keyboard window's geometry information
     */
    virtual void set_keyboard_size_hints(SclSize portrait, SclSize landscape);

    /**
     * @brief Requests to create an option window
     */
    virtual sclwindow create_option_window(SCLOptionWindowType type);

    /**
     * @brief Requests to destroy an option window
     */
    virtual void destroy_option_window(sclwindow window);

protected:
    std::string m_backend_identifier;

private:
    CSCLCoreUI *m_impl;
};

}

//SCL_END_DECLS

#endif //__SCL_CORE_UI_H__
