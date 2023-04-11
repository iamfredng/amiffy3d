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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#    include <Windows.h>
#endif

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

static bool quick = false;

void chinese_fix()
{
    SetConsoleOutputCP( 65001 );
}

void setup_nk_font( struct nk_glfw* glfw, struct nk_context* nk )
{
    struct nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin( glfw, &atlas );
    struct nk_font_config conf = nk_font_config( 96 );
    conf.range                 = nk_font_chinese_glyph_ranges();
    conf.oversample_h          = 2;
    conf.oversample_v          = 2;
    conf.pixel_snap            = 0;
    struct nk_font* f          = nk_font_atlas_add_from_file( atlas, "../../dyh.ttf", 18, &conf );
    nk_glfw3_font_stash_end( glfw );
    nk_style_set_font( nk, &f->handle );
}

void ExampleGLFWkeyfun( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    printf_s( "key: %d scancode: %d action: %d mods: %d\n", key, scancode, action, mods );
    if ( key == 81 && action == 0 ) {
        quick = TRUE;
    }
}

int main( int argc, char** argv )
{
#ifdef _WIN32
    chinese_fix();
#endif

    if ( !glfwInit() ) {
        printf_s( "初始化窗体失败\n" );
        exit( EXIT_FAILURE );
    }

    glfwWindowHint( GLFW_CONTEXT_DEBUG, GLFW_TRUE );
    glfwWindowHint( GLFW_SCALE_TO_MONITOR, GLFW_TRUE );
    glfwWindowHint( GLFW_WIN32_KEYBOARD_MENU, GLFW_TRUE );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );

    GLFWwindow* window = glfwCreateWindow( 600, 600, "Window 特性", NULL, NULL );

    if ( !window ) {
        printf_s( "创建窗体失败\n" );
        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    glfwMakeContextCurrent( window );
    gladLoadGL( glfwGetProcAddress );
    glfwSwapInterval( 1 );

    // glfwGetError( NULL );

    struct nk_glfw     glfw = { 0 };
    struct nk_context* nk   = nk_glfw3_init( &glfw, window, NK_GLFW3_INSTALL_CALLBACKS );

    setup_nk_font( &glfw, nk );
    // double lst = 0;

    glfwSetKeyCallback( window, ExampleGLFWkeyfun );
    while ( !quick && !glfwWindowShouldClose( window ) ) {
        int width, height;

        glfwGetWindowSize( window, &width, &height );

        struct nk_rect area = nk_rect( 0.f, 0.f, (float) width, (float) height );
        nk_window_set_bounds( nk, "窗体", area );
        nk_glfw3_new_frame( &glfw );

        if ( nk_begin( nk, "主窗体", area, 1 ) ) {
            nk_layout_row_dynamic( nk, 30, 5 );

            if ( nk_button_label( nk, "最大化" ) ) glfwMaximizeWindow( window );
            if ( nk_button_label( nk, "最小化" ) ) glfwIconifyWindow( window );
            if ( nk_button_label( nk, "Restore" ) ) glfwRestoreWindow( window );
            if ( nk_button_label( nk, "关闭" ) ) quick = true;
        }
        nk_end( nk );

        // double time = glfwGetTime();
        // printf_s( "Time: %f\n", time - lst );
        // lst = time;

        glClear( GL_COLOR_BUFFER_BIT );
        nk_glfw3_render( &glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER );
        glfwSwapBuffers( window );
        glClearColor( .0f, 1.0f, 1.0f, .0f );
        // glfwWaitEventsTimeout( 0.016f );
        // glfwWaitEvents();
        glfwPollEvents();
    }
    nk_glfw3_shutdown( &glfw );
    glfwTerminate();
    printf_s( "正常离开\n" );
    exit( EXIT_SUCCESS );
}
