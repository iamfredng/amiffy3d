#ifndef AMIFFY_AUDIO_H_
#define AMIFFY_AUDIO_H_
#include <irrKlang.h>

void open_audio_module();
void close_audio_module();
void play_audio( const char* fileName, bool loop );

void play_audio( const char* fileName, bool loop );

#endif