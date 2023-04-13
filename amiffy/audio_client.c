#define SOUNDIO_STATIC_LIBRARY
#include <soundio/soundio.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio_client.h"

#include <log.h>

const int BUFFER_SIZE = 128;

static void ( *write_sample )( char* ptr, double sample );

static const double             PI             = 3.14159265358979323846264338328;
static double                   seconds_offset = 0.0;
static volatile bool            want_pause     = false;
static struct SoundIoDevice*    outdevice;
static struct SoundIoOutStream* outstream;
static struct SoundIo*          soundio;

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

static void write_callback( struct SoundIoOutStream* out_stream, int frame_count_min, int frame_count_max )
{
    struct SoundIoChannelArea* areas;
    int                        err;
    double                     float_sample_rate = outstream->sample_rate;
    double                     seconds_per_frame = 1.0 / float_sample_rate;

    int frames_left = frame_count_max;

    for ( ;; ) {
        int frame_count = frames_left;
        if ( ( err = soundio_outstream_begin_write( out_stream, &areas, &frame_count ) ) ) {
            log_error( "unrecoverable stream error: %s", soundio_strerror( err ) );
            exit( 1 );
        }

        if ( !frame_count ) { break; }

        //int num_channels = out_stream->layout.channel_count;

        // Prepare the buffers for the app_read_callback
        //struct audio_area app_buffer_areas [2];

        //if ( instream->format == SoundIoFormatFloat32NE ) {
            // No conversion is necessary, give the areas directly to the app
            //for ( int i = 0; i < num_channels; ++i ) {
            //    app_buffer_areas [i] =
            //        Area( (float*) areas [i].ptr, frame_count, areas [i].step / sizeof( float ) );
            //}

            // Call the user_read_callback with the buffers
            //app_write_callback( frame_count, num_channels, app_buffer_areas );
        //}
        //else {
        //    // Conversion is necessary, create a custom buffer for the app to fill
        //    static float* app_buffer      = NULL;
        //    static size_t app_buffer_size = 0;

        //    size_t required_size = frame_count * num_channels;

        //    // Allocate the app buffer
        //    if ( !app_buffer || app_buffer_size < required_size ) {
        //        if ( app_buffer ) { delete [] app_buffer; }
        //        app_buffer      = new float [required_size];
        //        app_buffer_size = required_size;
        //    }

        //    // Create the app areas
        //    for ( int i = 0; i < num_channels; ++i ) {
        //        app_buffer_areas [i] = Area( app_buffer + i, frame_count, num_channels );
        //    }

        //    // Call the user_read_callback with the buffers
        //    app_write_callback( frame_count, num_channels, app_buffer_areas );

        //    // Copy back all the samples
        //    float* app_ptr = app_buffer;
            //for ( int i = 0; i < frame_count; ++i ) {
            //    for ( int j = 0; j < num_channels; ++j ) {
            //        write_sample( areas [j].ptr, *app_ptr++ );
            //        areas [j].ptr += areas [j].step;
            //    }
            //}
        //}

        const struct SoundIoChannelLayout* layout = &outstream->layout;

        double pitch              = 440.0;
        double radians_per_second = pitch * 2.0 * PI;
        for ( int frame = 0; frame < frame_count; frame += 1 ) {
            double sample = sin( ( seconds_offset + frame * seconds_per_frame  ) *
                                 radians_per_second );
            for ( int channel = 0; channel < layout->channel_count; channel += 1 ) {
                write_sample( areas [channel].ptr, sample  );
                areas [channel].ptr += areas [channel].step;
            }
        }
        seconds_offset = fmod( seconds_offset + seconds_per_frame * frame_count, 1.0 );

        if ( ( err = soundio_outstream_end_write( out_stream ) ) ) {
            if ( err == SoundIoErrorUnderflow ) return;
            fprintf( stderr, "unrecoverable stream error: %s\n", soundio_strerror( err ) );
            exit( 1 );
        }

        frames_left -= frame_count;
        if ( frames_left <= 0 ) { break; }
    }
}

int init_audio_client( int sample_rate, ReadCallback read_callback_, WriteCallback write_callback_ )
{
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
    if ( out_device_count < 0 ) { log_error( "no output device found" ); }

    log_debug( "out device count: %d", out_device_count );

    int default_out_device_index = soundio_default_output_device_index( soundio );
    if ( default_out_device_index < 0 ) { log_error( "no output device found" ); }

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

void destroy_audio_client() {
    soundio_outstream_destroy( outstream );
    soundio_device_unref( outdevice );
    soundio_destroy( soundio );
}