#ifndef AUDIO_CLIENT_H_
#define AUDIO_CLIENT_H_

struct audio_area
{
    float* ptr;
    float* end;
    int    step;
};

typedef void ( *ReadCallback )( int num_samples, int num_areas, struct audio_area* area );
typedef void ( *WriteCallback )( int num_samples, int num_areas, struct audio_area* area );

int  init_audio_client( int sample_rate, ReadCallback read_callback, WriteCallback write_callback );
void destroy_audio_client();

#endif   // !AUDIO_CLIENT_H_
