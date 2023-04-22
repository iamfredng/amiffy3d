<<<<<<<< HEAD:amiffy/main.cpp

#include "audio.h"
#include <iostream>

extern "C" {
#include "amiffy.h"
#include "scripts.h"
#include "ui.h"
}
static FILE* log_fd;

static void open_log_module()
{
    log_set_level( 0 );
    log_set_quiet( 0 );

    log_fd = fopen( AMIFFY_DEFAULT_LOG_FILE_PATH, "ab" );
    log_add_fp( log_fd, AMIFFY_DEFAULT_LOG_LEVEL );
}

static void close_log_module()
{
    log_info( "Log module closeing" );
    fclose( log_fd );
}

static void key_callback( int key, int scancode, int action, int mods )
{
    // alt   = mod4
    // shift = mod1
    // ctrl  = mod2
    // win   = mod8
    log_debug( "key: %d scancode: %d action: %d mods: %d", key, scancode, action, mods );
    if ( key == 294 && action == 0 ) {
        //        reload_lua = true;
        reload_script_module();
    }
    if ( key == 83 && action == 0 ) {
        play_audio( "./audio/explosion.wav", false );
    }
    if ( key == 65 && action == 0 ) {
        play_audio( "./audio/ophelia.mp3", false );
    }
}

int main( int argc, char** argv )
{
    open_log_module();
    log_info( "Amiffy Application is starting..............................." );

    open_ui_module();

    open_audio_module();

    open_script_module( AMIFFY_DEFAULT_WINDOW_WIDTH, AMIFFY_DEFAULT_WINDOW_HEIGHT );

    install_script_module();

    install_ui_script_module();

    init_ui_env();

    init_script_env();

    register_ui_frame_update_handler( update_script_frame );
    register_ui_window_key_callback( key_callback );

    begin_ui_event_loop();

    close_ui_module();

    close_script_module();

    close_audio_module();

    close_log_module();

    exit( EXIT_SUCCESS );
}
========
#include "amiffy.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
//#include <irrKlang.h>
#include "audio.h"
#include "scripts.h"
#include "ui.h"

static FILE* log_fd;



static void open_log_module()
{
    log_set_level( 0 );
    log_set_quiet( 0 );

    log_fd = fopen( AMIFFY_DEFAULT_LOG_FILE_PATH, "ab" );
    log_add_fp( log_fd, AMIFFY_DEFAULT_LOG_LEVEL );
}

static void close_log_module()
{
    log_info( "Log module closeing" );
    fclose( log_fd );
}

static void key_callback( int key, int scancode, int action, int mods )
{
    // alt   = mod4
    // shift = mod1
    // ctrl  = mod2
    // win   = mod8
    log_debug( "key: %d scancode: %d action: %d mods: %d", key, scancode, action, mods );
    if ( key == 294 && action == 0 ) {
        //        reload_lua = true;
        reload_script_module();
    }
}

int main( int argc, char** argv )
{
    open_log_module();
    log_info( "Amiffy Application is starting..............................." );

    open_ui_module();

//    open_audio_module();

    open_script_module( AMIFFY_DEFAULT_WINDOW_WIDTH, AMIFFY_DEFAULT_WINDOW_HEIGHT );

    install_script_module();

    install_ui_script_module();

    init_ui_env();

    init_script_env();

    register_ui_frame_update_handler( update_script_frame );
    register_ui_window_key_callback( key_callback );

    begin_ui_event_loop();

    close_ui_module();

    close_script_module();

//    close_audio_module();

    close_log_module();

    exit( EXIT_SUCCESS );
}
>>>>>>>> d255901 (替换声音模块为irrKlang):amiffy/main.c
