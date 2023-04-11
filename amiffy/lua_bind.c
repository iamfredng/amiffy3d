#include <log.h>
#include <nuklear/nuklear.h>

#include "amiffy.h"
#include "lua_bind.h"

int c_log_info( lua_State* L )
{
    const char* str = lua_tostring( L, 1 );
    log_info( "%s", str );
    return 0;
}

int amiffy_header( lua_State* L )
{
    // ui_header
    return 0;
}

int amiffy_end_window( lua_State* L )
{
    nk_end( nk );
    return 0;
}

int amiffy_begin_window( lua_State* L )
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

int amiffy_button( lua_State* L )
{
    const char* title = lua_tostring( L, 1 );
    nk_bool     rvl   = nk_button_label( nk, title );
    lua_pushboolean( L, rvl );
    return 1;
}

int amiffy_layout_row_push( lua_State* L )
{
    int num = lua_tonumber( L, 1 );
    nk_layout_row_push( nk, num );
    return 0;
}

int amiffy_layout_row_dynamic( lua_State* L )
{
    int height = lua_tonumber( L, 1 );
    int col    = lua_tonumber( L, 2 );
    nk_layout_row_dynamic( nk, height, col );
    return 0;
}

int amiffy_layout_row_static( lua_State* L )
{
    int height    = lua_tonumber( L, 1 );
    int itemWidth = lua_tonumber( L, 2 );
    int cols      = lua_tonumber( L, 3 );
    nk_layout_row_static( nk, height, itemWidth, cols );
    return 0;
}

bool bind_amiffy_func( lua_State* L )
{
    lua_register( L, "c_log_info", c_log_info );
    lua_register( L, "amiffy_begin_window", amiffy_begin_window );
    lua_register( L, "amiffy_end_window", amiffy_end_window );
    lua_register( L, "amiffy_button", amiffy_button );
    lua_register( L, "amiffy_layout_row_dynamic", amiffy_layout_row_dynamic );
    lua_register( L, "amiffy_layout_row_push", amiffy_layout_row_push );
    lua_register( L, "amiffy_layout_row_static", amiffy_layout_row_static );
    return true;
}
