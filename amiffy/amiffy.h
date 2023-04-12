#ifndef AMIFFY3D_H_
#define AMIFFY3D_H_

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define FONT_NAME "./fonts/harmony.ttf"
#define FONT_SIZE 16

// #define HIGHSPEED_EVENT_FLAG
#define LOWSPEED_EVENT_FLAG

extern struct nk_context* nk;
extern struct nk_colorf bg;

#endif