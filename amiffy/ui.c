#include "ui.h"

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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static bool quick = false;

static GLFWwindow*           window;
static struct nk_context*    nk;
static struct nk_font_atlas* atlas;
struct nk_colorf             bg;
static struct nk_glfw        glfw = { 0 };
struct media                 ui_media;
struct media
{
    GLint                skin;
    struct nk_image      menu;
    struct nk_image      check;
    struct nk_image      check_cursor;
    struct nk_image      option;
    struct nk_image      option_cursor;
    struct nk_nine_slice header;
    struct nk_nine_slice window;
    struct nk_image      scrollbar_inc_button;
    struct nk_image      scrollbar_inc_button_hover;
    struct nk_image      scrollbar_dec_button;
    struct nk_image      scrollbar_dec_button_hover;
    struct nk_image      button;
    struct nk_image      button_hover;
    struct nk_image      button_active;
    struct nk_image      tab_minimize;
    struct nk_image      tab_maximize;
    struct nk_image      slider;
    struct nk_image      slider_hover;
    struct nk_image      slider_active;
} ui_media;


#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

static window_key_callback  key_callback;
static frame_update_handler frame_handler;

static GLuint image_load( const char* filename )
{
    int            x, y, n;
    GLuint         tex;
    unsigned char* data = stbi_load( filename, &x, &y, &n, 0 );
    if ( !data ) log_warn( "failed to load image: %s", filename );

    glGenTextures( 1, &tex );
    glBindTexture( GL_TEXTURE_2D, tex );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
    glGenerateMipmap( GL_TEXTURE_2D );
    stbi_image_free( data );
    return tex;
}

void init_ui_style()
{
    glEnable( GL_TEXTURE_2D );
    ui_media.skin = image_load( AMIFFY_DEFAULT_SKIN_PATH );
    ui_media.window =
        nk_sub9slice_id( ui_media.skin, 512, 512, nk_rect( 0, 23, 127, 104 ), 4, 4, 4, 4 );
    ui_media.header =
        nk_sub9slice_id( ui_media.skin, 512, 512, nk_rect( 0, 0, 127, 23 ), 4, 4, 4, 4 );
    struct nk_color table [NK_COLOR_COUNT];
    table [NK_COLOR_TEXT]                    = nk_rgba( 190, 190, 190, 255 );
    table [NK_COLOR_WINDOW]                  = nk_rgba( 30, 33, 40, 215 );
    table [NK_COLOR_HEADER]                  = nk_rgba( 181, 45, 69, 220 );
    table [NK_COLOR_BORDER]                  = nk_rgba( 51, 55, 67, 255 );
    table [NK_COLOR_BUTTON]                  = nk_rgba( 75, 154, 235, 255 );
    table [NK_COLOR_BUTTON_HOVER]            = nk_rgba( 190, 50, 70, 255 );
    table [NK_COLOR_BUTTON_ACTIVE]           = nk_rgba( 195, 55, 75, 255 );
    table [NK_COLOR_TOGGLE]                  = nk_rgba( 51, 55, 67, 255 );
    table [NK_COLOR_TOGGLE_HOVER]            = nk_rgba( 45, 60, 60, 255 );
    table [NK_COLOR_TOGGLE_CURSOR]           = nk_rgba( 181, 45, 69, 255 );
    table [NK_COLOR_SELECT]                  = nk_rgba( 51, 55, 67, 255 );
    table [NK_COLOR_SELECT_ACTIVE]           = nk_rgba( 181, 45, 69, 255 );
    table [NK_COLOR_SLIDER]                  = nk_rgba( 51, 55, 67, 255 );
    table [NK_COLOR_SLIDER_CURSOR]           = nk_rgba( 181, 45, 69, 255 );
    table [NK_COLOR_SLIDER_CURSOR_HOVER]     = nk_rgba( 186, 50, 74, 255 );
    table [NK_COLOR_SLIDER_CURSOR_ACTIVE]    = nk_rgba( 191, 55, 79, 255 );
    table [NK_COLOR_PROPERTY]                = nk_rgba( 51, 55, 67, 255 );
    table [NK_COLOR_EDIT]                    = nk_rgba( 51, 55, 67, 225 );
    table [NK_COLOR_EDIT_CURSOR]             = nk_rgba( 190, 190, 190, 255 );
    table [NK_COLOR_COMBO]                   = nk_rgba( 51, 55, 67, 255 );
    table [NK_COLOR_CHART]                   = nk_rgba( 51, 55, 67, 255 );
    table [NK_COLOR_CHART_COLOR]             = nk_rgba( 170, 40, 60, 255 );
    table [NK_COLOR_CHART_COLOR_HIGHLIGHT]   = nk_rgba( 255, 0, 0, 255 );
    table [NK_COLOR_SCROLLBAR]               = nk_rgba( 30, 33, 40, 255 );
    table [NK_COLOR_SCROLLBAR_CURSOR]        = nk_rgba( 64, 84, 95, 255 );
    table [NK_COLOR_SCROLLBAR_CURSOR_HOVER]  = nk_rgba( 70, 90, 100, 255 );
    table [NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba( 75, 95, 105, 255 );
    table [NK_COLOR_TAB_HEADER]              = nk_rgba( 181, 45, 69, 220 );
    nk_style_from_table( nk, table );

    /* window */
    nk->style.window.background              = nk_rgb( 204, 204, 204 );
    nk->style.window.fixed_background        = nk_style_item_nine_slice( ui_media.window );
    nk->style.window.border_color            = nk_rgb( 67, 67, 67 );
    nk->style.window.combo_border_color      = nk_rgb( 67, 67, 67 );
    nk->style.window.contextual_border_color = nk_rgb( 67, 67, 67 );
    nk->style.window.menu_border_color       = nk_rgb( 67, 67, 67 );
    nk->style.window.group_border_color      = nk_rgb( 67, 67, 67 );
    nk->style.window.tooltip_border_color    = nk_rgb( 67, 67, 67 );
    nk->style.window.scrollbar_size          = nk_vec2( 16, 16 );
    nk->style.window.border_color            = nk_rgba( 0, 0, 0, 0 );
    nk->style.window.padding                 = nk_vec2( 2, 2 );
    nk->style.window.border                  = 3;

    /* window header */
    nk->style.window.header.normal       = nk_style_item_nine_slice( ui_media.header );
    nk->style.window.header.hover        = nk_style_item_nine_slice( ui_media.header );
    nk->style.window.header.active       = nk_style_item_nine_slice( ui_media.header );
    nk->style.window.header.label_normal = nk_rgb( 95, 95, 95 );
    nk->style.window.header.label_hover  = nk_rgb( 95, 95, 95 );
    nk->style.window.header.label_active = nk_rgb( 95, 95, 95 );

    nk->style.text.color = nk_rgb( 95, 95, 95 );
}

static void text_input( GLFWwindow* win, unsigned int codepoint )
{
    nk_input_unicode( (struct nk_context*) glfwGetWindowUserPointer( win ), codepoint );
}

static void scroll_input( GLFWwindow* win, double _, double yoff )
{
    (void) _;
    nk_input_scroll( (struct nk_context*) glfwGetWindowUserPointer( win ),
                     nk_vec2( 0, (float) yoff ) );
}

static void error_callback( int e, const char* d )
{
    printf( "Error %d: %s\n", e, d );
}

void setup_nk_font( struct nk_glfw* glfw_, struct nk_context* nk_ )
{
    log_info( "设置字体" );

    nk_glfw3_font_stash_begin( glfw_, &atlas );

    struct nk_font_config conf = nk_font_config( AMIFFY_DEFAULT_FONT_SIZE );
    conf.range                 = nk_font_chinese_glyph_ranges();
    conf.oversample_h          = 2;
    conf.oversample_v          = 2;
    conf.pixel_snap            = 0;
    struct nk_font* f          = nk_font_atlas_add_from_file(
        atlas, AMIFFY_DEFAULT_FONT_NAME, AMIFFY_DEFAULT_FONT_SIZE, &conf );

    nk_glfw3_font_stash_end( glfw_ );
    nk_style_set_font( nk_, &f->handle );
}

void glfwKeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    if ( key_callback != NULL ) {
        key_callback( key, scancode, action, mods );
    }
}

void open_ui_module()
{
    glfwSetErrorCallback( error_callback );

    if ( !glfwInit() ) {
        log_error( "initialization window system failed\n" );
        exit( EXIT_FAILURE );
    }

    // glfwDefaultWindowHints();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_DEBUG, GLFW_TRUE );
    glfwWindowHint( GLFW_SCALE_TO_MONITOR, GLFW_FALSE );
    glfwWindowHint( GLFW_WIN32_KEYBOARD_MENU, GLFW_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    // 透明无边框窗体, 最好配合全屏
    // glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
    // glfwWindowHint( GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE );
    // glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    //     glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );

    window = glfwCreateWindow(
        AMIFFY_DEFAULT_WINDOW_WIDTH, AMIFFY_DEFAULT_WINDOW_HEIGHT, "Window 窗体", NULL, NULL );
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

    nk = nk_glfw3_init( &glfw, window, NK_GLFW3_INSTALL_CALLBACKS );

    setup_nk_font( &glfw, nk );

    init_ui_style();

    log_info( "GUI system initialized" );

    glfwSetKeyCallback( window, glfwKeyCallback );
    log_info( "key input event initialized" );
}

void close_ui_module()
{
    nk_glfw3_shutdown( &glfw );

    nk_free( nk );

    glfwTerminate();
    log_info( "destory glfw" );
}

void abort_ui_event_loop()
{
    quick = true;
}

void begin_ui_event_loop()
{
    int width, height;
    while ( !quick && !glfwWindowShouldClose( window ) ) {
        glClearColor( bg.r, bg.g, bg.b, bg.a );
        glClear( GL_COLOR_BUFFER_BIT );
        nk_glfw3_render( &glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER );
        glfwSwapBuffers( window );

        {
#ifdef HIGHSPEED_EVENT_FLAG
            glfwPollEvents();
#endif

#ifdef LOWSPEED_EVENT_FLAG
            glfwWaitEvents();
            // glfwWaitEventsTimeout( 0.016f );
#endif
        }

        nk_glfw3_new_frame( &glfw );
        glfwGetFramebufferSize( window, &width, &height );

        if ( frame_handler != NULL ) {
//            frame_handler( width, height );
        }

    }
}

void register_ui_window_key_callback( window_key_callback keyCallback )
{
    key_callback = keyCallback;
}

void register_ui_frame_update_handler( frame_update_handler handler )
{
    frame_handler = handler;
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

void install_ui_script_module()
{
    lua_getglobal( lua_state, "package" );
    lua_getfield( lua_state, -1, "preload" );

    lua_pushcfunction( lua_state, luaopen_imgui );
    lua_setfield( lua_state, -2, "imgui" );

    lua_pop( lua_state, 2 );
}

void init_ui_env()
{
    lua_pushnumber( lua_state, NK_WINDOW_BORDER );
    lua_setglobal( lua_state, "NK_WINDOW_BORDER" );
    lua_pushnumber( lua_state, NK_WINDOW_MOVABLE );
    lua_setglobal( lua_state, "NK_WINDOW_MOVABLE" );
    lua_pushnumber( lua_state, NK_WINDOW_SCALABLE );
    lua_setglobal( lua_state, "NK_WINDOW_SCALABLE" );
    lua_pushnumber( lua_state, NK_WINDOW_CLOSABLE );
    lua_setglobal( lua_state, "NK_WINDOW_CLOSABLE" );
    lua_pushnumber( lua_state, NK_WINDOW_MINIMIZABLE );
    lua_setglobal( lua_state, "NK_WINDOW_MINIMIZABLE" );
    lua_pushnumber( lua_state, NK_WINDOW_NO_SCROLLBAR );
    lua_setglobal( lua_state, "NK_WINDOW_NO_SCROLLBAR" );
    lua_pushnumber( lua_state, NK_WINDOW_TITLE );
    lua_setglobal( lua_state, "NK_WINDOW_TITLE" );
    lua_pushnumber( lua_state, NK_WINDOW_SCROLL_AUTO_HIDE );
    lua_setglobal( lua_state, "NK_WINDOW_SCROLL_AUTO_HIDE" );
    lua_pushnumber( lua_state, NK_WINDOW_BACKGROUND );
    lua_setglobal( lua_state, "NK_WINDOW_BACKGROUND" );
    lua_pushnumber( lua_state, NK_WINDOW_SCALE_LEFT );
    lua_setglobal( lua_state, "NK_WINDOW_SCALE_LEFT" );
    lua_pushnumber( lua_state, NK_WINDOW_NO_INPUT );
    lua_setglobal( lua_state, "NK_WINDOW_NO_INPUT" );
}