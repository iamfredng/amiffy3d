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

static int amiffy_button( lua_State* L )
{
    const char* title = lua_tostring( L, 1 );
    nk_bool     rvl   = nk_button_label( nk, title );
    lua_pushboolean( L, rvl );
    return 1;
}

static int amiffy_layout_row_dynamic( lua_State* L )
{
    int height = lua_tonumber( L, 1 );
    int col    = lua_tonumber( L, 2 );
    nk_layout_row_dynamic( nk, height, col );
    return 0;
}

void bind_amiffy_func( lua_State* L )
{
    lua_register( L, "c_log_info", c_log_info );
    lua_register( L, "amiffy_button", amiffy_button );
    lua_register( L, "amiffy_layout_row_dynamic", amiffy_layout_row_dynamic );
}
