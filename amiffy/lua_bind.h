#ifndef LUA_BIND_H_
#define LUA_BIND_H_
#include <stdbool.h>
#include <log.h>

#include "amiffy.h"
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lapi.h>

#include "lua_print_stack.h"
bool bind_amiffy_modules( lua_State* L );
#endif
