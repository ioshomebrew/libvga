#include <stdio.h>
#include <stdlib.h>
#include <sys/nearptr.h>
#include <dos.h>

#define sgn(x) ((x<0)?-1:((x>0)?1:0))

typedef unsigned char byte;
typedef unsigned short word;

extern byte *VGA;

void set_mode(int mode);
void plot_pixel_bios(int x, int y, byte color);
void plot_pixel_memory(int x, int y, byte color);
void line_slow(int x1, int y1, int x2, int y2, byte color);
void line_fast(int x1, int y1, int x2, int y2, byte color);
void polygon(int num_vertices, int *vertices, byte color);
void rect_slow(int left, int top, int right, int bottom, byte color);
void rect_fast(int left, int top, int right, int bottom, byte color);


