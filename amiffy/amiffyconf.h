//
// Created by iamfr on 2023/4/24.
//

#ifndef AMIFFY_AMIFFYCONF_H_
#define AMIFFY_AMIFFYCONF_H_

#include <lua/lua.h>

#define AMIFFY_DEFAULT_SCRIPT_NAME "init"
#define AMIFFY_DEFAULT_INIT_FUNC_NAME "setup"
#define AMIFFY_DEFAULT_UPDATE_FUNC_NAME "update"

#define AMIFFY_DEFAULT_LOG_FILE_PATH "./log_debug.txt"
#define AMIFFY_DEFAULT_LOG_LEVEL LOG_DEBUG

#define AMIFFY_DEFAULT_WINDOW_WIDTH 600
#define AMIFFY_DEFAULT_WINDOW_HEIGHT 600

#define AMIFFY_DEFAULT_FONT_NAME "./fonts/harmony.ttf"
#define AMIFFY_DEFAULT_FONT_SIZE 16

#define AMIFFY_DEFAULT_SKIN_PATH "./assets/skins/gwen.png"

// #define HIGHSPEED_EVENT_FLAG
#define LOWSPEED_EVENT_FLAG

extern lua_State* lua_state;
#endif   // AMIFFY_AMIFFYCONF_H_