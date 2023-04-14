#include <log.h>
#include <nuklear/nuklear.h>

#include "amiffy.h"
#include "lua_bind.h"
#include <lua/lauxlib.h>
#include <lua/lua.h>

#include "lua_print_stack.h"

static int c_log_debug( lua_State* L )
{
    const char* str = lua_tostring( L, 1 );
    log_debug( "%s", str );
    return 0;
}

static int c_log_warn( lua_State* L )
{
    const char* str = lua_tostring( L, 1 );
    log_warn( "%s", str );
    return 0;
}

static int c_log_error( lua_State* L )
{
    const char* str = lua_tostring( L, 1 );
    log_error( "%s", str );
    return 0;
}

static int c_log_info( lua_State* L )
{
    const char* str = lua_tostring( L, 1 );
    log_info( "%s", str );
    return 0;
}

static int imgui_end_window( lua_State* L )
{
    nk_end( nk );
    return 0;
}

static int imgui_begin_window( lua_State* L )
{
    const char* winname = lua_tostring( L, 1 );
    float       x       = lua_tonumber( L, 2 );
    float       y       = lua_tonumber( L, 3 );
    float       width   = lua_tonumber( L, 4 );
    float       height  = lua_tonumber( L, 5 );
    int         flags   = lua_tonumber( L, 6 );

    bool rvl = nk_begin( nk, winname, nk_rect( x, y, width, height ), flags );
    lua_pushboolean( L, rvl );
    return 1;
}

static int imgui_button( lua_State* L )
{
    const char* title = lua_tostring( L, 1 );
    nk_bool     rvl   = nk_button_label( nk, title );
    lua_pushboolean( L, rvl );
    return 1;
}

static int imgui_layout_row_push( lua_State* L )
{
    float num = lua_tonumber( L, 1 );
    nk_layout_row_push( nk, num );
    return 0;
}

static int imgui_layout_row_dynamic( lua_State* L )
{
    float height = lua_tonumber( L, 1 );
    int   col    = lua_tonumber( L, 2 );
    nk_layout_row_dynamic( nk, height, col );
    return 0;
}

static int imgui_layout_row_static( lua_State* L )
{
    float height    = lua_tonumber( L, 1 );
    int   itemWidth = lua_tonumber( L, 2 );
    int   cols      = lua_tonumber( L, 3 );
    nk_layout_row_static( nk, height, itemWidth, cols );
    return 0;
}

static int imgui_window_is_collapsed( lua_State* L )
{
    const char* windowName  = lua_tostring( L, 1 );
    nk_bool     isCollapsed = nk_window_is_collapsed( nk, windowName );
    lua_pushboolean( L, isCollapsed );
    return 1;
}

static int imgui_change_bg_color( lua_State* L )
{
    float r = lua_tonumber( L, 1 );
    float g = lua_tonumber( L, 2 );
    float b = lua_tonumber( L, 3 );
    float a = lua_tonumber( L, 4 );

    bg.a = a;
    bg.r = r;
    bg.g = g;
    bg.b = b;

    return 0;
}

static int luaopen_log( lua_State* L )
{
    luaL_Reg log [] = { { "info", c_log_info },
                        { "debug", c_log_debug },
                        { "warn", c_log_warn },
                        { "error", c_log_error },
                        { NULL, NULL } };
    luaL_newlib( L, log );

    log_info( "初始化log模块" );

    return 1;
}

static int luaopen_imgui( lua_State* L )
{
    luaL_Reg l [] = { { "change_bg_color", imgui_change_bg_color },
                      { "begin_window", imgui_begin_window },
                      { "end_window", imgui_end_window },
                      { "button", imgui_button },
                      { "layout_row_dynamic", imgui_layout_row_dynamic },
                      { "layout_row_static", imgui_layout_row_static },
                      { "layout_row_push", imgui_layout_row_push },
                      { "window_is_collapsed", imgui_window_is_collapsed },
                      { NULL, NULL } };
    luaL_newlib( L, l );

    log_info( "初始化imgui模块" );

    return 1;
}

static int c_print_stack( lua_State* L )
{
    print_stack( L );
    return 0;
}

static bool bind_amiffy_modules( lua_State* L )
{
    lua_register( L, "c_log_info", c_log_info );
    lua_register( L, "c_print_stack", c_print_stack );

    lua_getglobal( L, "package" );
    lua_getfield( L, -1, "preload" );

    lua_pushcfunction( L, luaopen_imgui );
    lua_setfield( L, -2, "imgui" );

    lua_pushcfunction( L, luaopen_log );
    lua_setfield( L, -2, "log" );
    lua_pop( L, 2 );
    return true;
}
