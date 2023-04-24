//
// Created by iamfr on 2023/4/24.
//
#include "amiffy.h"

extern "C" {
#include "amiffyconf.h"
#include <log.h>
}

namespace Amiffy {

void Amiffy::openAudioModule()
{
    engine = irrklang::createIrrKlangDevice();
    if ( !engine ) return;   // error starting up the engine
}

void Amiffy::closeAudioModule()
{
    engine->drop();
}

void Amiffy::playAudio( const char* fileName, bool loop )
{
    engine->play2D( fileName, loop );
}

void Amiffy::globalKeyCallback(int key, int scancode, int action, int mods)
{
    // alt   = mod4
    // shift = mod1
    // ctrl  = mod2
    // win   = mod8
    log_debug( "key: %d scancode: %d action: %d mods: %d", key, scancode, action, mods );
    if ( key == 294 && action == 0 ) {
        //        reload_lua = true;
//        reload_script_module();
    }
    if ( key == 83 && action == 0 ) {
        playAudio( "./audio/explosion.wav", false );
    }
    if ( key == 65 && action == 0 ) {
        playAudio( "./audio/ophelia.mp3", false );
    }
}

void Amiffy::openLogModule()
{
    log_set_level( 0 );
    log_set_quiet( false );

    log_fd = fopen( AMIFFY_DEFAULT_LOG_FILE_PATH, "ab" );
    log_add_fp( log_fd, AMIFFY_DEFAULT_LOG_LEVEL );
}

void Amiffy::closeLogModule()
{
    log_info( "Log module closeing" );
    fclose( log_fd );
}

void Amiffy::run() {}
}   // namespace Amiffy