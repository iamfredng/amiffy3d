#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear/nuklear.h>
#include <nuklear/nuklear_glfw_gl3.h>

#include <log.h>

#include <lua/lapi.h>
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#    include <Windows.h>
#endif

#include "amiffy.h"
#include "lua_bind.h"

static bool           quick = false;
static lua_State*     lua_state;
static FILE*          log_fd;
struct nk_context*    nk;
struct nk_font_atlas* atlas;
struct nk_colorf      bg;

void setup_lua()
{
    lua_state = luaL_newstate();
    luaL_openlibs( lua_state );
    luaopen_debug( lua_state );

    bind_amiffy_modules( lua_state );

    /// 嵌入一些简单的辅助函数, 方便不需要在init.lua中定义或额外写定义文件
    int rvl = luaL_dostring( lua_state, "_G['NK_FLAG'] = function(inv) return 1 << inv end;" );

    rvl = luaL_dofile( lua_state, "lua/init.lua" );
    if ( rvl != 0 ) {
        log_error( "dofile 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
        exit( EXIT_FAILURE );
    }

    lua_getglobal( lua_state, "init" );
    rvl = lua_pcall( lua_state, 0, 0, 0 );
    if ( rvl != 0 ) {
        log_error( "init 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
        exit( EXIT_FAILURE );
    }
    log_info( "lua环境初始化完毕" );
}

void setup_nk_font( struct nk_glfw* glfw, struct nk_context* nk )
{
    log_info( "设置字体" );

    nk_glfw3_font_stash_begin( glfw, &atlas );

    struct nk_font_config conf = nk_font_config( FONT_SIZE );
    conf.range                 = nk_font_chinese_glyph_ranges();
    conf.oversample_h          = 2;
    conf.oversample_v          = 2;
    conf.pixel_snap            = 0;
    struct nk_font* f          = nk_font_atlas_add_from_file( atlas, FONT_NAME, FONT_SIZE, &conf );

    nk_glfw3_font_stash_end( glfw );
    nk_style_set_font( nk, &f->handle );
}

void glfwKeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    log_debug( "key: %d scancode: %d action: %d mods: %d", key, scancode, action, mods );
}

void setup_log()
{
    log_set_level( 0 );
    log_set_quiet( 0 );

    log_fd = fopen( "./log_debug.txt", "ab" );
    log_add_fp( log_fd, LOG_DEBUG );
}

void lua_update_call( int width, int height )
{
    lua_getglobal( lua_state, "update" );
    lua_pushnumber( lua_state, width );
    lua_pushnumber( lua_state, height );
    int rvl = lua_pcall( lua_state, 2, 0, 0 );
    if ( rvl != 0 ) {
        log_error( "lua_update_call 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
    }
}

static void text_input( GLFWwindow* win, unsigned int codepoint )
{
    nk_input_unicode( (struct nk_context*) glfwGetWindowUserPointer( win ), codepoint );
}

static void scroll_input( GLFWwindow* win, double _, double yoff )
{
    UNUSED( _ );
    nk_input_scroll( (struct nk_context*) glfwGetWindowUserPointer( win ),
                     nk_vec2( 0, (float) yoff ) );
}

static void error_callback( int e, const char* d )
{
    printf( "Error %d: %s\n", e, d );
}

int main( int argc, char** argv )
{
    setup_log();

    setup_lua();
    glfwSetErrorCallback( error_callback );

    if ( !glfwInit() ) {
        log_error( "初始化窗体失败\n" );
        exit( EXIT_FAILURE );
    }

    // glfwDefaultWindowHints();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_DEBUG, GLFW_TRUE );
    glfwWindowHint( GLFW_SCALE_TO_MONITOR, GLFW_TRUE );
    glfwWindowHint( GLFW_WIN32_KEYBOARD_MENU, GLFW_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    // glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
    // glfwWindowHint( GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE );
    // glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    // glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );

    GLFWwindow* window = glfwCreateWindow( 600, 600, "Window", NULL, NULL );
    if ( !window ) {
        log_error( "创建窗体失败\n" );
        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    glfwMakeContextCurrent( window );
    glfwSetCharCallback( window, text_input );
    glfwSetScrollCallback( window, scroll_input );

    gladLoadGL( glfwGetProcAddress );
    glfwSwapInterval( 1 );
    glfwSetWindowAttrib( window, GLFW_TRANSPARENT_FRAMEBUFFER, 1 );

    log_info( "初始化窗体完成" );

    log_info( "初始化GUI系统" );
    struct nk_glfw glfw = { 0 };
    nk                  = nk_glfw3_init( &glfw, window, NK_GLFW3_INSTALL_CALLBACKS );
    setup_nk_font( &glfw, nk );

    glfwSetKeyCallback( window, glfwKeyCallback );
    log_info( "绑定按键回调" );

    int width, height;

    while ( !quick && !glfwWindowShouldClose( window ) ) {

#ifdef HIGHSPEED_EVENT_FLAG
        glfwPollEvents();
#endif

#ifdef LOWSPEED_EVENT_FLAG
        glfwWaitEvents();
        // glfwWaitEventsTimeout( 0.016f );
#endif
        nk_glfw3_new_frame( &glfw );

        glfwGetWindowSize( window, &width, &height );

        lua_update_call( width, height );
        struct nk_rect area = nk_rect( 0.f, 0.f, 280, 220 );
        nk_window_set_bounds( nk, "窗体", area );

        if ( nk_begin( nk,
                       "主窗体",
                       nk_rect( 0.f, 0.f, 280, 220 ),
                       NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                           NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE ) ) {
            nk_layout_row_dynamic( nk, 30, 4 );
            if ( nk_button_label( nk, "最大化" ) ) glfwMaximizeWindow( window );
            if ( nk_button_label( nk, "最小化" ) ) glfwIconifyWindow( window );
            if ( nk_button_label( nk, "Restore" ) ) glfwRestoreWindow( window );
            if ( nk_button_label( nk, "关闭" ) ) quick = true;
        }
        nk_end( nk );

        glfwGetWindowSize( window, &width, &height );
        glViewport( 0, 0, width, height );
        glClearColor( bg.r, bg.g, bg.b, bg.a );
        glClear( GL_COLOR_BUFFER_BIT );

        nk_glfw3_render( &glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER );
        glfwSwapBuffers( window );
    }

    nk_glfw3_shutdown( &glfw );

    nk_free( nk );

    glfwTerminate();
    log_info( "销毁glfw" );

    lua_close( lua_state );
    log_info( "销毁lua虚拟机" );

    log_info( "正常离开\n" );

    fclose( log_fd );
    exit( EXIT_SUCCESS );
}
