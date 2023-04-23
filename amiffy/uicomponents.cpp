//
// Created by iamfr on 2023/4/23.
//
#include <imgui.h>

extern "C" {
#include "uicomponents.h"
#include <log.h>
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}

static int amiffy_begin_window( lua_State* L )
{
    const char* str    = lua_tostring( L, 1 );
    bool        active = lua_toboolean( L, 2 );

    bool rvl = ImGui::Begin( str, &active, 0 );

    //    log_info( "%s", str );

    lua_pushboolean( L, rvl );
    return 1;
}

static int amiffy_end_window( lua_State* L )
{
    ImGui::End();
    return 0;
}

static int amiffy_button( lua_State* L )
{
    const char* str       = lua_tostring( L, 1 );
    bool        isClicked = ImGui::Button( str );

    lua_pushboolean( L, isClicked );
    return 1;
}

static int amiffy_text( lua_State* L )
{
    const char* str = lua_tostring( L, 1 );
    ImGui::Text( str );
    return 0;
}

static int luaopen_imgui( lua_State* L )
{
    luaL_Reg log [] = { { "begin_window", amiffy_begin_window },
                        { "end_window", amiffy_end_window },
                        { "button", amiffy_button },
                        { "text", amiffy_text },
                        { NULL, NULL } };
    luaL_newlib( L, log );

    log_info( "初始化imgui模块" );

    return 1;
}