///////////////////////////////////////////
// my_bmp.h - Header file for my_bmp.c
//
// Author: Mike Field <hamster@snap.net.nz>
///////////////////////////////////////////
int mybmp_read(unsigned char **data, int *width, int *height, char *filename);
int mybmp_write(unsigned char *data, int width, int height, char *filename);
