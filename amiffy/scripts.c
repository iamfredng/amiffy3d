#include "amiffy.h"

#include "printstack.h"
#include "scripts.h"

#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>

static lua_State* lua_state;

static bool reload_lua = false;

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

static int c_print_stack( lua_State* L )
{
    print_stack( L );
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

static int msghandler( lua_State* L )
{
    const char* msg = lua_tostring( L, 1 );
    if ( msg == NULL ) {                            /* is error object not a string? */
        if ( luaL_callmeta( L, 1, "__tostring" ) && /* does it have a metamethod */
             lua_type( L, -1 ) == LUA_TSTRING )     /* that produces a string? */
            return 1;                               /* that is the message */
        else
            msg = lua_pushfstring( L, "(error object is a %s value)", luaL_typename( L, 1 ) );
    }
    luaL_traceback( L, L, msg, 1 ); /* append a standard traceback */
    return 1;                       /* return the traceback */
}

static int docall( lua_State* L, int narg, int nres )
{
    int status;
    int base = lua_gettop( L ) - narg;  /* function index */
    lua_pushcfunction( L, msghandler ); /* push message handler */
    lua_insert( L, base );              /* put it under function and args */
    status = lua_pcall( L, narg, nres, base );
    lua_remove( L, base );              /* remove message handler from the stack */
    return status;
}

void hotreload_lua()
{
    luaL_dostring( lua_state,
                   "_G['amiffy'] = reload('init'); _G['amiffy_init'] = amiffy.init; "
                   "_G['amiffy_update'] = amiffy.update" );

    lua_getglobal( lua_state, "amiffy_init" );
    int rvl = docall( lua_state, 0, 0 );
    if ( rvl != 0 ) {
        log_error( "reload init 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
    }
}

static void init_script_env()
{
    // 设置基础函数和nk的窗体常量
    luaL_dostring(
        lua_state,
        "package.cpath=package.cpath .. ';./clib/?.dll;./clib/?.so'"
        "function reload(module) package.loaded[module] = nil; return require(module); end;" );

    // 初始化脚本
    int rvl = luaL_dostring( lua_state,
                             "_G['amiffy'] = require('init'); _G['amiffy_init'] = amiffy.init; "
                             "_G['amiffy_update'] = amiffy.update" );
    if ( rvl != 0 ) {
        log_error( "init 2失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
    }
    // 加载init.lua的初始化函数
    lua_getglobal( lua_state, "amiffy_init" );
    rvl = docall( lua_state, 0, 0 );
    if ( rvl != 0 ) {
        log_error( "init 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
    }
}

static void open_script_module( double width, double height )
{
    log_info( "Opening script module......" );

    lua_state = luaL_newstate();
    luaL_openlibs( lua_state );

    lua_newtable( lua_state );
    lua_pushnumber( lua_state, width );
    lua_setfield( lua_state, -2, "width" );
    lua_pushnumber( lua_state, height );
    lua_setfield( lua_state, -2, "height" );
    lua_setglobal( lua_state, "window" );

    log_debug( "lua_state: %p", lua_state );
}

static void close_script_module()
{
    lua_close( lua_state );
    log_info( "Script module closed" );
}

static void reload_script_module()
{
    reload_lua = true;
}

void update_script_frame( double width, double height )
{
    if ( reload_lua ) {
        reload_lua = false;
        hotreload_lua();

        return;
    }

    lua_getglobal( lua_state, "window" );
    lua_pushnumber( lua_state, width );
    lua_setfield( lua_state, -2, "width" );
    lua_pushnumber( lua_state, height );
    lua_setfield( lua_state, -2, "height" );
    lua_pop( lua_state, 1 );

    lua_getglobal( lua_state, "amiffy_update" );

    int rvl = docall( lua_state, 0, 0 );
    if ( rvl != 0 ) {
        log_error( "lua_update_call 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
    }
}

static void install_script_module()
{
    lua_register( lua_state, "c_log_info", c_log_info );
    lua_register( lua_state, "c_print_stack", c_print_stack );

    lua_getglobal( lua_state, "package" );
    lua_getfield( lua_state, -1, "preload" );

    lua_pushcfunction( lua_state, luaopen_log );
    lua_setfield( lua_state, -2, "log" );
    lua_pop( lua_state, 2 );
}
