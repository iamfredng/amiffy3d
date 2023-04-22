#ifndef AMIFFY_UI_H_
#define AMIFFY_UI_H_

typedef void ( *frame_update_handler )( double width, double height );
typedef void ( *window_key_callback )( int key, int scancode, int action, int mods );

void open_ui_module();

void begin_ui_event_loop();

void abort_ui_event_loop();

void register_ui_window_key_callback( window_key_callback );

void register_ui_frame_update_handler( frame_update_handler );

void install_ui_script_module();

void init_ui_env();

void close_ui_module();

#endif