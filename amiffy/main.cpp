#include <iostream>

#include "amiffy.h"
#include "scripts.h"
#include "ui.h"
#include "uicomponents.h"

extern "C" {
#include <log.h>
}

int main( int argc, char** argv )
{
    Amiffy::Amiffy             amiffy;
    Amiffy::AmiffyUI           amiffyUI( &amiffy );
    Amiffy::AmiffyUIComponents amiffyUiComponents( &amiffyUI );
    Amiffy::AmiffyScriptModule amiffyScriptModule( &amiffy );

    amiffy.openLogModule();
    log_info( "Amiffy Application is starting..............................." );

    amiffyUI.registerUIFrameUpdate( [&amiffyScriptModule]( int width, int height ) {
        amiffyScriptModule.tick( width, height );
    } );

    amiffyUI.createWindow();
    amiffyUI.openUIModule();

    amiffy.openAudioModule();

    amiffyScriptModule.openScriptModule();

    //        open_script_module( AMIFFY_DEFAULT_WINDOW_WIDTH, AMIFFY_DEFAULT_WINDOW_HEIGHT );
    amiffyScriptModule.installScriptModule();
    //    install_script_module();

    amiffyUI.initUIEnv();
    //    initUIEnv();

    amiffyScriptModule.initScriptEnv();
    //    init_script_env();

    //    register_ui_frame_update_handler( update_script_frame );
    //    register_ui_window_key_callback( &amiffy );

    amiffyUI.beginUIEventLoop();

    amiffyUI.closeUIModule();
    amiffyUI.desotryWindow();

    //    close_script_module();

    amiffy.closeAudioModule();

    amiffy.closeLogModule();

    exit( EXIT_SUCCESS );
}