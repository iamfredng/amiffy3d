#include "scripts.h"

extern "C" {
#include "printstack.h"
#include <log.h>
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}


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
                        { nullptr, nullptr } };
    luaL_newlib( L, log );

    log_info( "初始化log模块" );

    return 1;
}


namespace Amiffy {

static int msghandler( lua_State* L )
{
    const char* msg = lua_tostring( L, 1 );
    if ( msg == nullptr ) {                            /* is error object not a string? */
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

void AmiffyScriptModule::openScriptModule()
{
    log_info( "Opening script module......" );
    auto lua_state = luaL_newstate();
    luaL_openlibs( lua_state );

    amiffy->luaVm = lua_state;
    vm            = lua_state;
    log_debug( "lua_state: %p", lua_state );
}

void AmiffyScriptModule::reloadScriptModule()
{
    auto lua_state = (lua_State*) vm;
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

void AmiffyScriptModule::closeScriptModule()
{
    lua_close( (lua_State*) vm );
    log_info( "Script module closed" );
}

void AmiffyScriptModule::installScriptModule()
{
    auto lua_state = (lua_State*) vm;

    lua_register( lua_state, "c_log_info", c_log_info );
    lua_register( lua_state, "c_print_stack", c_print_stack );

    lua_getglobal( lua_state, "package" );
    lua_getfield( lua_state, -1, "preload" );

    lua_pushcfunction( lua_state, luaopen_log );
    lua_setfield( lua_state, -2, "log" );
    lua_pop( lua_state, 2 );
}

void AmiffyScriptModule::tick( int clientWidth, int clientHeight )
{
    auto lua_state = (lua_State*) vm;

    lua_getglobal( lua_state, "window" );
    lua_pushnumber( lua_state, clientWidth );
    lua_setfield( lua_state, -2, "width" );
    lua_pushnumber( lua_state, clientHeight );
    lua_setfield( lua_state, -2, "height" );
    lua_pop( lua_state, 1 );

    lua_getglobal( lua_state, "amiffy_update" );

    int rvl = docall( lua_state, 0, 0 );
    if ( rvl != 0 ) {
        log_error( "lua_update_call 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
    }
}

void AmiffyScriptModule::initScriptEnv()
{
    auto lua_state = (lua_State*) vm;
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

AmiffyScriptModule::AmiffyScriptModule( Amiffy* _amiffy )
{
    amiffy = _amiffy;
}

AmiffyScriptModule::~AmiffyScriptModule()
{
    amiffy = nullptr;
}
}   // namespace Amiffy
