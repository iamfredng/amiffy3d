#include "amiffy.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


// #include "audio_client.h"
#include "scripts.h"

#include "ui.h"

// #include "stb_img.h"
//#include <stb_vorbis.h>
static FILE* log_fd;



float* output;
short* input;

// static struct audio_area file_left_channel;
// static struct audio_area file_right_channel;

// static bool reload_lua = false;

//
//  static void setup_media_file()
//{
//     ui_media.skin          = load_img( "./assets/skins/gwen.png" );
//     ui_media.check         = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 464, 32, 15, 15 )
//     ); ui_media.check_cursor  = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 450, 34, 11, 11
//     ) ); ui_media.option        = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 464, 64, 15,
//     15 ) ); ui_media.option_cursor = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 451, 67,
//     9, 9 ) ); ui_media.header        = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 128, 0,
//     127, 24 ) ); ui_media.window        = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 128,
//     23, 127, 104 ) ); ui_media.scrollbar_inc_button =
//         nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 464, 256, 15, 15 ) );
//     ui_media.scrollbar_inc_button_hover =
//         nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 464, 320, 15, 15 ) );
//     ui_media.scrollbar_dec_button =
//         nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 464, 224, 15, 15 ) );
//     ui_media.scrollbar_dec_button_hover =
//         nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 464, 288, 15, 15 ) );
//     ui_media.button        = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 384, 336, 127, 31
//     )
//     ); ui_media.button_hover  = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 384, 368, 127,
//     31 ) ); ui_media.button_active = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 384, 400,
//     127, 31 ) ); ui_media.tab_minimize  = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 451,
//     99, 9, 9 ) ); ui_media.tab_maximize  = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 467,
//     99, 9, 9 ) ); ui_media.slider        = nk_subimage_id( ui_media.skin, 512, 512, nk_rect( 418,
//     33, 11, 14 ) ); ui_media.slider_hover  = nk_subimage_id( ui_media.skin, 512, 512, nk_rect(
//     418, 49, 11, 14 ) ); ui_media.slider_active = nk_subimage_id( ui_media.skin, 512, 512,
//     nk_rect( 418, 64, 11, 14 ) );
// }


static void open_log_module()
{
    log_set_level( 0 );
    log_set_quiet( 0 );

    log_fd = fopen( AMIFFY_DEFAULT_LOG_FILE_PATH, "ab" );
    log_add_fp( log_fd, AMIFFY_DEFAULT_LOG_LEVEL );
}

static void close_log_module()
{
    log_info( "Log module closeing" );
    fclose( log_fd );
}

static void key_callback( int key, int scancode, int action, int mods )
{
    // alt   = mod4
    // shift = mod1
    // ctrl  = mod2
    // win   = mod8
    log_debug( "key: %d scancode: %d action: %d mods: %d", key, scancode, action, mods );
    if ( key == 294 && action == 0 ) {
        //        reload_lua = true;
        abort_ui_event_loop();
    }
}

//
// void audio_read_callback( int num_samples, int num_areas, struct audio_area* areas ) {}
// void audio_write_callback( int num_samples, int num_areas, struct audio_area* areas )
//{
//    for ( int n = 0; n < num_areas; ++n ) {
//        struct audio_area area = areas [n];
//        while ( area.ptr < area.end ) {
//            *area.ptr++ = 0.0;
//        }
//    }
//
//    struct audio_area* in_l  = &file_left_channel;
//    struct audio_area* in_r  = &file_right_channel;
//    struct audio_area  out_l = areas [0];
//    struct audio_area  out_r = areas [1];
//    while ( in_l->ptr < in_l->end && out_l.ptr < out_l.end ) {
//        *out_l.ptr++ = *in_l->ptr++;
//        *out_r.ptr++ = *in_r->ptr++;
//    }
//}
//
// inline float convert_sample( short sample )
//{
//    return (float) ( (double) sample / 32768.0 );
//}
//
// void convert_samples( int num_samples, short* input_, float* output_ )
//{
//    short* in_ptr     = input_;
//    short* in_ptr_end = input_ + num_samples;
//    float* out_ptr    = output_;
//    while ( in_ptr < in_ptr_end ) {
//        *out_ptr++ = convert_sample( *in_ptr++ );
//    }
//}

int main( int argc, char** argv )
{
    open_log_module();
    log_info( "Amiffy Application is starting..............................." );

    open_ui_module();



    // ------------------------------------------------------------ ogg test

    //    const char* file_name = "D:/c/amiffy3d/audio/1000.ogg";
    //
    //    int num_channels, sample_rate;
    //    int num_samples = stb_vorbis_decode_filename( file_name, &num_channels, &sample_rate,
    //    &input );
    //
    //    log_debug( "sample_rate: %d", sample_rate );
    //    log_debug( "num_channels: %d", num_channels );
    //    log_debug( "num_samples: %d", num_samples );
    //
    //    output = (float*) malloc( num_samples * num_channels * sizeof( float ) );
    //    convert_samples( num_samples * num_channels, input, output );
    //    free( input );
    //
    //    file_left_channel.ptr  = output;
    //    file_left_channel.end  = file_left_channel.ptr + num_samples * num_channels;
    //    file_left_channel.step = 2;
    //
    //    file_right_channel.ptr  = output + 1;
    //    file_right_channel.end  = file_right_channel.ptr + num_samples * num_channels;
    //    file_right_channel.step = 2;

    // ------------------------------------------------------------ end ogg test

    //    init_audio_client( sample_rate, audio_read_callback, audio_write_callback );

    //    if ( reload_lua ) {
    //        reload_lua = false;
    //        hotreload_lua();
    //    }
    //    else {
    //        lua_update_call( width, height );
    //        update_script_frame( width, height );
    //    }

    open_script_module( AMIFFY_DEFAULT_WINDOW_WIDTH, AMIFFY_DEFAULT_WINDOW_HEIGHT );

    install_script_module();

    install_ui_script_module();

    init_ui_env();

    init_script_env();

    register_ui_frame_update_handler( update_script_frame );
    register_ui_window_key_callback( key_callback );

    begin_ui_event_loop();

    close_ui_module();

    close_script_module();

    close_log_module();
    //
    //    destroy_audio_client();

    exit( EXIT_SUCCESS );
}
