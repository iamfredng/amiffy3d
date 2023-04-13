#ifndef STB_VORBIS_H_
#define STB_VORBIS_H_
int stb_vorbis_decode_filename(const char *filename, int *channels, int *sample_rate, short **output);
#endif