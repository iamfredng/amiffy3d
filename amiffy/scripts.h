#ifndef AMIFFY_SCRIPT_H_
#define AMIFFY_SCRIPT_H_

#include <lua/lauxlib.h>
#include <lua/lua.h>

void open_script_module( double width, double height );

void install_script_module();

void init_script_env();

void update_script_frame( double width, double height );

void close_script_module();

#endif