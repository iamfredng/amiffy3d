#ifndef AMIFFY_AUDIO_H_
#define AMIFFY_AUDIO_H_

#include <irrKlang.h>
#include <string>

namespace Amiffy {
class AmiffyAudio
{
public:
    void openAudioModule();
    void closeAudioModule();
    void installAudioModule();
    void playAudio( std::string fileName, bool loop = false );

private:
    irrklang::ISoundEngine* soundEngine;
};
}   // namespace Amiffy


#endif