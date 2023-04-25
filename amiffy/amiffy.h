#ifndef AMIFFY3D_H_
#define AMIFFY3D_H_

#include <iostream>

#include "audio.h"

namespace Amiffy {

typedef void* LuaVM;


class Amiffy
{
public:
    void openLogModule();
    void closeLogModule();

    void openAudioModule();
    void closeAudioModule();
    void playAudio( const char* fileName, bool loop );

    void globalKeyCallback( int key, int scancode, int action, int mods );

    void run();

    Amiffy();
    virtual ~Amiffy();

public:
    LuaVM        luaVm  = nullptr;

private:
    AmiffyAudio* audio  = nullptr;
    FILE*        log_fd = nullptr;
};
};   // namespace Amiffy

#endif
