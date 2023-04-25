#include "audio.h"
#include <irrKlang.h>

namespace Amiffy {
using namespace irrklang;

void AmiffyAudio::openAudioModule()
{
    soundEngine = createIrrKlangDevice();
}

void AmiffyAudio::closeAudioModule()
{
    soundEngine->drop();
}

void AmiffyAudio::playAudio( std::string fileName, bool loop )
{
    soundEngine->play2D( fileName.c_str(), loop );
}

void AmiffyAudio::installAudioModule()
{

}
}   // namespace Amiffy