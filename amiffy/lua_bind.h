#ifndef LUA_BIND_H_
#define LUA_BIND_H_

#include <lua/lua.h>

void       bind_amiffy_func( lua_State* L );
int        c_log_info( lua_State* L );
static int amiffy_button( lua_State* L );
static int amiffy_layout_row_dynamic( lua_State* L );
#endif