#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef _WIN32
#    include <Windows.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
#include "amiffy.h"
#include "audio_client.h"
#include "lua_bind.h"
#include <log.h>
#include <lua/lapi.h>
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
#include <nuklear/nuklear.h>
#include <nuklear/nuklear_glfw_gl3.h>
#include <stb_vorbis.h>

static bool           quick = false;
static lua_State*     lua_state;
static FILE*          log_fd;
struct nk_context*    nk;
struct nk_font_atlas* atlas;
struct nk_colorf      bg;
struct nk_glfw        glfw = { 0 };

static bool reload_lua = false;

void hotreload_lua()
{
    luaL_dostring(lua_state, "_G['amiffy_init'] = reload('init'); _G['init'] = amiffy_init.init; _G['update'] = amiffy_init.update");

    lua_getglobal( lua_state, "init" );
    int rvl = lua_pcall( lua_state, 0, 0, 0 );
    if ( rvl != 0 ) {
        log_error( "reload init 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
        exit( EXIT_FAILURE );
    }
}

void init_lua()
{
    // 设置基础函数和nk的窗体常量
    luaL_dostring(lua_state, "function reload(module) package.loaded[module] = nil; return require(module); end; _G['NK_FLAG'] = function(inv) return 1 << inv end; NK_WINDOW_BORDER = NK_FLAG(0);NK_WINDOW_MOVABLE = NK_FLAG(1);NK_WINDOW_SCALABLE = NK_FLAG(2);NK_WINDOW_CLOSABLE = NK_FLAG(3);NK_WINDOW_MINIMIZABLE = NK_FLAG(4);NK_WINDOW_NO_SCROLLBAR = NK_FLAG(5);NK_WINDOW_TITLE = NK_FLAG(6);NK_WINDOW_SCROLL_AUTO_HIDE = NK_FLAG(7);NK_WINDOW_BACKGROUND = NK_FLAG(8);NK_WINDOW_SCALE_LEFT = NK_FLAG(9);NK_WINDOW_NO_INPUT = NK_FLAG(10); ");

    // 初始化脚本
    int rvl = luaL_dostring( lua_state, "_G['amiffy_init'] = require('init'); _G['init'] = amiffy_init.init; _G['update'] = amiffy_init.update" );
    if ( rvl != 0 ) {
        log_error( "init 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
        exit( EXIT_FAILURE );
    }

    // 加载init.lua的初始化函数
    lua_getglobal( lua_state, "init" );
    rvl = lua_pcall( lua_state, 0, 0, 0 );
    if ( rvl != 0 ) {
        log_error( "init 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
        exit( EXIT_FAILURE );
    }
}

void setup_lua()
{
    lua_state = luaL_newstate();
    luaL_openlibs( lua_state );
    luaopen_debug( lua_state );

    bind_amiffy_modules( lua_state );

    lua_newtable(lua_state);
    lua_pushnumber(lua_state, 0);
    lua_setfield(lua_state, -2, "width");
    lua_pushnumber(lua_state, 0);
    lua_setfield(lua_state, -2, "height");
    lua_setglobal(lua_state, "window");

    init_lua();
}

void lua_update_call( int width, int height )
{
    lua_getglobal(lua_state, "window");
    lua_pushnumber(lua_state, width);
    lua_setfield(lua_state, -2, "width");
    lua_pushnumber(lua_state, height);
    lua_setfield(lua_state, -2, "height");

    lua_getglobal( lua_state, "update" );
    int rvl = lua_pcall( lua_state, 0, 0, 0 );
    if ( rvl != 0 ) {
        log_error( "lua_update_call 失败, %d", rvl );
        log_error( "%s", lua_tostring( lua_state, -1 ) );
    }
}

void setup_nk_font( struct nk_glfw* glfw_, struct nk_context* nk_ )
{
    log_info( "设置字体" );

    nk_glfw3_font_stash_begin( glfw_, &atlas );

    struct nk_font_config conf = nk_font_config( FONT_SIZE );
    conf.range                 = nk_font_chinese_glyph_ranges();
    conf.oversample_h          = 2;
    conf.oversample_v          = 2;
    conf.pixel_snap            = 0;
    struct nk_font* f          = nk_font_atlas_add_from_file( atlas, FONT_NAME, FONT_SIZE, &conf );

    nk_glfw3_font_stash_end( glfw_ );
    nk_style_set_font( nk_, &f->handle );
}

void glfwKeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    // alt   = mod4
    // shift = mod1
    // ctrl  = mod2
    // win   = mod8
    // log_debug( "key: %d scancode: %d action: %d mods: %d", key, scancode, action, mods );
    if ( key == 294 && action == 0 ) {
        reload_lua = true;
    }
}

static void setup_log()
{
    log_set_level( 0 );
    log_set_quiet( 0 );


    log_fd = fopen( "./log_debug.txt", "ab" );
    log_add_fp( log_fd, LOG_DEBUG );

    log_info( "Amiffy Application is starting..............................." );
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

float* output;
short* input;

static struct audio_area file_left_channel;
static struct audio_area file_right_channel;

void audio_read_callback( int num_samples, int num_areas, struct audio_area* areas ) {}

void audio_write_callback( int num_samples, int num_areas, struct audio_area* areas )
{
    for ( int n = 0; n < num_areas; ++n ) {
        struct audio_area area = areas [n];
        while ( area.ptr < area.end ) {
            *area.ptr++ = 0.0;
        }
    }

    struct audio_area* in_l  = &file_left_channel;
    struct audio_area* in_r  = &file_right_channel;
    struct audio_area  out_l = areas [0];
    struct audio_area  out_r = areas [1];
    while ( in_l->ptr < in_l->end && out_l.ptr < out_l.end ) {
        *out_l.ptr++ = *in_l->ptr++;
        *out_r.ptr++ = *in_r->ptr++;
    }
}

inline float convert_sample( short sample )
{
    return (float) ( (double) sample / 32768.0 );
}

void convert_samples( int num_samples, short* input_, float* output_ )
{
    short* in_ptr     = input_;
    short* in_ptr_end = input_ + num_samples;
    float* out_ptr    = output_;
    while ( in_ptr < in_ptr_end ) {
        *out_ptr++ = convert_sample( *in_ptr++ );
    }
}

int main( int argc, char** argv )
{
    setup_log();

    glfwSetErrorCallback( error_callback );

    if ( !glfwInit() ) {
        log_error( "initialization window system failed\n" );
        exit( EXIT_FAILURE );
    }

    // glfwDefaultWindowHints();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_DEBUG, GLFW_TRUE );
    glfwWindowHint( GLFW_SCALE_TO_MONITOR, GLFW_TRUE );
    glfwWindowHint( GLFW_WIN32_KEYBOARD_MENU, GLFW_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    // 透明无边框窗体, 最好配合全屏
    // glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
    // glfwWindowHint( GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE );
    // glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    // glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );

    GLFWwindow* window = glfwCreateWindow( 600, 600, "Window 窗体", NULL, NULL );
    if ( !window ) {
        log_error( "main windows initialization failed\n" );
        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    glfwMakeContextCurrent( window );
    glfwSetCharCallback( window, text_input );
    glfwSetScrollCallback( window, scroll_input );

    gladLoadGL( glfwGetProcAddress );
    glfwSwapInterval( 1 );
    glfwSetWindowAttrib( window, GLFW_TRANSPARENT_FRAMEBUFFER, 1 );

    log_info( "window system initialized" );

    log_info( "GUI system initialized" );

    nk = nk_glfw3_init( &glfw, window, NK_GLFW3_INSTALL_CALLBACKS );
    setup_nk_font( &glfw, nk );

    glfwSetKeyCallback( window, glfwKeyCallback );
    log_info( "key input event initialized" );

    // ------------------------------------------------------------ ogg test

    const char* file_name = "D:/c/amiffy3d/audio/1000.ogg";

    int num_channels, sample_rate;

    int num_samples = stb_vorbis_decode_filename( file_name, &num_channels, &sample_rate, &input );

    log_debug( "sample_rate: %d", sample_rate );
    log_debug( "num_channels: %d", num_channels );
    log_debug( "num_samples: %d", num_samples );

    output = (float*) malloc( num_samples * num_channels * sizeof( float ) );
    convert_samples( num_samples * num_channels, input, output );
    free( input );

    file_left_channel.ptr  = output;
    file_left_channel.end  = file_left_channel.ptr + num_samples * num_channels;
    file_left_channel.step = 2;

    file_right_channel.ptr  = output + 1;
    file_right_channel.end  = file_right_channel.ptr + num_samples * num_channels;
    file_right_channel.step = 2;

    // ------------------------------------------------------------ end ogg test

    init_audio_client( sample_rate, audio_read_callback, audio_write_callback );

    setup_lua();

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

        if ( reload_lua ) {
            reload_lua = false;
            hotreload_lua();
        }
        else {
            lua_update_call( width, height );
        }

        glfwGetFramebufferSize( window, &width, &height );
        glViewport( 0, 0, width, height );
        glClearColor( bg.r, bg.g, bg.b, bg.a );
        glClear( GL_COLOR_BUFFER_BIT );

        nk_glfw3_render( &glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER );
        glfwSwapBuffers( window );
    }

    nk_glfw3_shutdown( &glfw );

    nk_free( nk );

    glfwTerminate();
    log_info( "destory glfw" );

    lua_close( lua_state );
    log_info( "destory lua VM" );

    log_info( "exit application" );

    fclose( log_fd );

    destroy_audio_client();

    exit( EXIT_SUCCESS );
}
