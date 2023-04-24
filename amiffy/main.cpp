#include <iostream>

#include "amiffyconf.h"
#include "amiffy.h"
#include "ui.h"

extern "C" {
#include "scripts.h"
#include <log.h>
}

int main( int argc, char** argv )
{
    Amiffy::Amiffy amiffy;

    amiffy.openLogModule();
    log_info( "Amiffy Application is starting..............................." );

    open_ui_module();

    amiffy.openAudioModule();

    open_script_module( AMIFFY_DEFAULT_WINDOW_WIDTH, AMIFFY_DEFAULT_WINDOW_HEIGHT );

    install_script_module();

    install_ui_script_module();

    init_ui_env();

    init_script_env();

    register_ui_frame_update_handler( update_script_frame );
    register_ui_window_key_callback( &amiffy );

    begin_ui_event_loop();

    close_ui_module();

    close_script_module();

    amiffy.closeAudioModule();

    amiffy.closeLogModule();

    exit( EXIT_SUCCESS );
}