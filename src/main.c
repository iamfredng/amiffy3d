#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
// #include <wchar.h>

#ifdef _WIN32
#    include <Windows.h>
#endif

// int main( int argc, char** argv )
//{
//     printf( "Amiffy3D text" );
//     return 0;
// }

void error_callback( int error, const char* desc )
{
    fprintf( stderr, "ERROR: %s\n", desc );
}

void chinese_fix()
{
    SetConsoleOutputCP( 65001 );
}

int main( int argc, char** argv )
{
#ifdef _WIN32
    chinese_fix();
#endif

    int  last_xpos = INT_MIN, last_ypos = INT_MIN;
    int  last_width = INT_MIN, last_height = INT_MIN;
    char width_buffer [12] = "", height_buffer [12] = "";
    char xpos_buffer [12] = "", ypos_buffer [12] = "";

    if ( !glfwInit() ) {
        printf( "初始化窗体失败\n" );
        exit( EXIT_FAILURE );
    }

    glfwWindowHint( GLFW_SCALE_TO_MONITOR, GLFW_TRUE );
    glfwWindowHint( GLFW_WIN32_KEYBOARD_MENU, GLFW_TRUE );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );

    GLFWwindow* window = glfwCreateWindow( 600, 600, "Window 特性", NULL, NULL );

    if ( !window ) {
        printf_s( "创建窗体失败\n" );
        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    printf_s( "创建窗体完成\n" );

    glfwMakeContextCurrent( window );
    gladLoadGL( glfwGetProcAddress );
    glfwSwapInterval( 0 );

    glfwGetError( NULL );
    glfwGetWindowPos( window, &last_xpos, &last_ypos );
    sprintf( xpos_buffer, "%i", last_xpos );
    sprintf( ypos_buffer, "%i", last_ypos );

    glfwGetWindowSize( window, &last_width, &last_height );
    sprintf( width_buffer, "%i", last_width );
    sprintf( height_buffer, "%i", last_height );

    while ( !glfwWindowShouldClose( window ) ) {
        int width, height;

        glfwGetWindowSize( window, &width, &height );

        printf_s( "Window Size: %d %d\n", width, height );

        glfwSwapBuffers( window );

        glClear( GL_COLOR_BUFFER_BIT );
        glClearColor( 1.0f, 0.0f, .0f, .0f );
        glfwWaitEvents();
    }
    glfwTerminate();
    printf_s( "正常离开\n" );
    exit( EXIT_SUCCESS );
}
