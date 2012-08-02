#ifndef LIBVGA_H
#define LIBVGA_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/nearptr.h>
#include <dos.h>
#include <math.h>

#define sgn(x) ((x<0)?-1:((x>0)?1:0))

typedef unsigned char byte;
typedef unsigned short word;
typedef long           fixed16_16;

fixed16_16 SIN_ACOS[1024];

extern byte *VGA;
extern word *my_clock;

typedef struct BITMAP
{
 word width;
 word height;
 byte *data;
} BITMAP;

void set_mode(int mode);
void plot_pixel_bios(int x, int y, byte color);
void plot_pixel_memory(int x, int y, byte color);
void line_slow(int x1, int y1, int x2, int y2, byte color);
void line_fast(int x1, int y1, int x2, int y2, byte color);
void polygon(int num_vertices, int *vertices, byte color);
void rect_slow(int left, int top, int right, int bottom, byte color);
void rect_fast(int left, int top, int right, int bottom, byte color);
void rect_fill(int left, int top, int right, int bottom, byte color);
void circle_slow(int x, int y, int radius, byte color);
void circle_fast(int x, int y, int radius, byte color);
void circle_fill(int x, int y, int radius, byte color);

// bitmap support
void fskip(FILE *fp, int num_bytes);
void load_bmp(char *file, BITMAP *b);
void draw_bitmap(BITMAP *bmp,int x,int y);
void draw_transparent_bitmap(BITMAP *bmp, int x, int y);
void wait(int ticks);
#endif