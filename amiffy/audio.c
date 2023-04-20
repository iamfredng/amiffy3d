#include "audio.h"

#define SOUNDIO_STATIC_LIBRARY
#include <soundio/soundio.h>

#include <stb_vorbis.h>

#include "amiffy.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct audio_area
{
    float* ptr;
    float* end;
    int    step;
};

typedef void ( *ReadCallback )( int num_samples, int num_areas, struct audio_area* area );
typedef void ( *WriteCallback )( int num_samples, int num_areas, struct audio_area* area );

static struct audio_area file_left_channel;
static struct audio_area file_right_channel;

static struct SoundIoChannelArea* areas;

static WriteCallback app_write_callback;

const int BUFFER_SIZE = 128;

static struct SoundIoDevice*    outdevice;
static struct SoundIoOutStream* outstream;
static struct SoundIo*          soundio;

float* output;
short* input;

static void ( *write_sample )( char* ptr, double sample );

static void write_sample_s16ne( char* ptr, double sample )
{
    int16_t* buf   = (int16_t*) ptr;
    double   range = (double) INT16_MAX - (double) INT16_MIN;
    double   val   = sample * range / 2.0;
    *buf           = val;
}

static void write_sample_s32ne( char* ptr, double sample )
{
    int32_t* buf   = (int32_t*) ptr;
    double   range = (double) INT32_MAX - (double) INT32_MIN;
    double   val   = sample * range / 2.0;
    *buf           = val;
}

static void write_sample_float32ne( char* ptr, double sample )
{
    float* buf = (float*) ptr;
    *buf       = sample;
}

static void write_sample_float64ne( char* ptr, double sample )
{
    double* buf = (double*) ptr;
    *buf        = sample;
}

static void underflow_callback( struct SoundIoOutStream* out_stream )
{
    static int count = 0;
    log_error( "underflow %d", count++ );
}


static void write_callback( struct SoundIoOutStream* out_stream, int frame_count_min,
                            int frame_count_max )
{
    int    err;
    double float_sample_rate = out_stream->sample_rate;
    double seconds_per_frame = 1.0 / float_sample_rate;

    int frames_left = frame_count_max;

    for ( ;; ) {
        int frame_count = frames_left;
        if ( ( err = soundio_outstream_begin_write( out_stream, &areas, &frame_count ) ) ) {
            log_error( "unrecoverable stream error: %s", soundio_strerror( err ) );
            exit( 1 );
        }

        if ( !frame_count ) {
            break;
        }

        int num_channels = out_stream->layout.channel_count;

        // Prepare the buffers for the app_read_callback
        struct audio_area* app_buffer_areas =
            (struct audio_area*) malloc( num_channels * sizeof( struct audio_area ) );

        // Conversion is necessary, create a custom buffer for the app to fill
        static float* app_buffer      = NULL;
        static size_t app_buffer_size = 0;

        size_t required_size = frame_count * num_channels;

        // Allocate the app buffer
        if ( !app_buffer || app_buffer_size < required_size ) {
            if ( app_buffer ) {
                free( app_buffer );
            }
            app_buffer      = (float*) malloc( required_size * sizeof( float ) );
            app_buffer_size = required_size;
        }

        //// Create the app areas
        for ( int i = 0; i < num_channels; ++i ) {
            struct audio_area a;
            a.ptr               = app_buffer + i;
            a.end               = a.ptr + frame_count * num_channels;
            a.step              = num_channels;
            *app_buffer_areas++ = a;
            //*app_buffer_areas++ = audio_a ( app_buffer + i, frame_count, num_channels );
        }

        // Call the user_read_callback with the buffers
        app_write_callback( frame_count, num_channels, app_buffer_areas - num_channels );

        // Copy back all the samples
        float* app_ptr = app_buffer;
        for ( int i = 0; i < frame_count; ++i ) {
            for ( int j = 0; j < num_channels; ++j ) {
                write_sample( areas [j].ptr, *app_ptr++ );
                areas [j].ptr += areas [j].step;
            }
        }


        if ( ( err = soundio_outstream_end_write( out_stream ) ) ) {
            if ( err == SoundIoErrorUnderflow ) return;
            fprintf( stderr, "unrecoverable stream error: %s\n", soundio_strerror( err ) );
            exit( 1 );
        }

        frames_left -= frame_count;
        if ( frames_left <= 0 ) {
            break;
        }
    }
}

inline float convert_sample( short sample )
{
    return (float) ( (double) sample / 32768.0 );
}

void convert_samples( int num_samples, short* input_, float* output_ )
{
    short* in_ptr     = input_;
    short* in_ptr_end = input_ + num_samples;
    float* out_ptr    = output_;
    while ( in_ptr < in_ptr_end ) {
        *out_ptr++ = convert_sample( *in_ptr++ );
    }
}

void audio_read_callback( int num_samples, int num_areas, struct audio_area* areas ) {}
void audio_write_callback( int num_samples, int num_areas, struct audio_area* areas )
{
    for ( int n = 0; n < num_areas; ++n ) {
        struct audio_area area = areas [n];
        while ( area.ptr < area.end ) {
            *area.ptr++ = 0.0;
        }
    }

    struct audio_area* in_l  = &file_left_channel;
    struct audio_area* in_r  = &file_right_channel;
    struct audio_area  out_l = areas [0];
    struct audio_area  out_r = areas [1];
    while ( in_l->ptr < in_l->end && out_l.ptr < out_l.end ) {
        *out_l.ptr++ = *in_l->ptr++;
        *out_r.ptr++ = *in_r->ptr++;
    }
}


static int init_audio_client( int sample_rate, ReadCallback read_callback_,
                              WriteCallback write_callback_ )
{
    app_write_callback = write_callback_;

    double latency = (double) BUFFER_SIZE / sample_rate;

    int err;
    soundio = soundio_create();
    if ( !soundio ) {
        log_error( "out of memory" );
        return 1;
    }

    if ( ( err = soundio_connect( soundio ) ) ) {
        log_error( "error connecting: %s", soundio_strerror( err ) );
        return 1;
    }

    soundio_flush_events( soundio );

    int out_device_count = soundio_output_device_count( soundio );
    if ( out_device_count < 0 ) {
        log_error( "no output device found" );
    }

    log_debug( "out device count: %d", out_device_count );

    int default_out_device_index = soundio_default_output_device_index( soundio );
    if ( default_out_device_index < 0 ) {
        log_error( "no output device found" );
    }

    struct SoundIoDevice* device = soundio_get_output_device( soundio, default_out_device_index );
    if ( !device ) {
        log_error( "out of memory" );
        return 1;
    }

    log_info( "Output device: %s", device->name );

    outstream = soundio_outstream_create( device );
    if ( !outstream ) {
        log_error( "out of memory" );
        return 1;
    }

    outstream->write_callback     = write_callback;
    outstream->underflow_callback = underflow_callback;
    outstream->name               = device->name;
    outstream->software_latency   = latency;
    outstream->sample_rate        = sample_rate;

    if ( soundio_device_supports_format( device, SoundIoFormatFloat32NE ) ) {
        outstream->format = SoundIoFormatFloat32NE;
        write_sample      = write_sample_float32ne;
    }
    else if ( soundio_device_supports_format( device, SoundIoFormatFloat64NE ) ) {
        outstream->format = SoundIoFormatFloat64NE;
        write_sample      = write_sample_float64ne;
    }
    else if ( soundio_device_supports_format( device, SoundIoFormatS32NE ) ) {
        outstream->format = SoundIoFormatS32NE;
        write_sample      = write_sample_s32ne;
    }
    else if ( soundio_device_supports_format( device, SoundIoFormatS16NE ) ) {
        outstream->format = SoundIoFormatS16NE;
        write_sample      = write_sample_s16ne;
    }
    else {
        log_error( "No suitable device format available." );
        return 1;
    }

    if ( ( err = soundio_outstream_open( outstream ) ) ) {
        log_error( "unable to open device: %s", soundio_strerror( err ) );
    }

    log_error( "Input software latency: %f", outstream->software_latency );

    if ( outstream->layout_error ) {
        log_error( "unable to set channel layout: %s",
                   soundio_strerror( outstream->layout_error ) );
    }

    if ( ( err = soundio_outstream_start( outstream ) ) ) {
        log_error( "unable to start device: %s", soundio_strerror( err ) );
    }

    outdevice = device;

    return 0;
}

static void destroy_audio_client()
{
    soundio_outstream_destroy( outstream );
    soundio_device_unref( outdevice );
    soundio_destroy( soundio );
}

static void open_audio_module()
{
    // ------------------------------------------------------------ ogg test

    const char* file_name = "D:/c/amiffy3d/audio/1000.ogg";

    int num_channels, sample_rate;
    int num_samples = stb_vorbis_decode_filename( file_name, &num_channels, &sample_rate, &input );

    log_debug( "sample_rate: %d", sample_rate );
    log_debug( "num_channels: %d", num_channels );
    log_debug( "num_samples: %d", num_samples );

    output = (float*) malloc( num_samples * num_channels * sizeof( float ) );
    convert_samples( num_samples * num_channels, input, output );
    free( input );

    file_left_channel.ptr  = output;
    file_left_channel.end  = file_left_channel.ptr + num_samples * num_channels;
    file_left_channel.step = 2;

    file_right_channel.ptr  = output + 1;
    file_right_channel.end  = file_right_channel.ptr + num_samples * num_channels;
    file_right_channel.step = 2;

    // ------------------------------------------------------------ end ogg test

    init_audio_client( sample_rate, audio_read_callback, audio_write_callback );
}

static void close_audio_module()
{
    free( output );
}