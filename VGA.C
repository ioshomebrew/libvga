#include "libvga.h"

int main()
{
 int x1,y1,x2,y2,color;
 float t1,t2;
 word i,start;

 set_mode(0x13);
 VGA+=__djgpp_conventional_base;

 plot_pixel_memory(100,100,1);

 line_slow(5, 5, 10, 10, 5);

 rect_slow(200, 50, 250, 50, 13);

 int num_vertices=3;
 int vertices[6]={5,0,
                  7,5,
                  1,4};
 polygon(3,vertices,15);
 

 while(getchar() != '\n');

 set_mode(0x03);

 return 0;
}
