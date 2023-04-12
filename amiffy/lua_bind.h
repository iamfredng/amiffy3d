#ifndef LUA_BIND_H_
#define LUA_BIND_H_
#include <lua/lua.h>
#include <stdbool.h>

bool bind_amiffy_modules( lua_State* L );

int c_log_info( lua_State* L );
int c_log_debug( lua_State* L );
int c_log_warn( lua_State* L );
int c_log_error( lua_State* L );

int imgui_change_bg_color( lua_State* L );

int imgui_begin_window( lua_State* L );
int imgui_end_window( lua_State* L );
int imgui_button( lua_State* L );
int imgui_layout_row_push( lua_State* L );
int imgui_layout_row_dynamic( lua_State* L );
int imgui_layout_row_static( lua_State* L );

#endif
