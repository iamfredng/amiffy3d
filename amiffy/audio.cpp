#include "audio.h"
#include <irrKlang.h>

using namespace irrklang;

static ISoundEngine* engine;

void open_audio_module()
{
    engine = createIrrKlangDevice();
    if ( !engine ) return;   // error starting up the engine
}

void close_audio_module()
{
    engine->drop();
}


void play_audio( const char* fileName, bool loop )
{
    engine->play2D( fileName, loop );
}