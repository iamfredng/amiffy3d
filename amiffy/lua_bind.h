#ifndef LUA_BIND_H_
#define LUA_BIND_H_
#include <lua/lua.h>
#include <stdbool.h>


bool bind_amiffy_func( lua_State* L );
int  c_log_info( lua_State* L );
int  amiffy_begin_window( lua_State* L );
int  amiffy_end_window( lua_State* L );
int  amiffy_button( lua_State* L );
int  amiffy_layout_row_push( lua_State* L );
int  amiffy_layout_row_dynamic( lua_State* L );
int  amiffy_layout_row_static( lua_State* L );
#endif
