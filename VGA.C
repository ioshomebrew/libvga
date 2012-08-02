#include "libvga.h"

int main()
{
 int x,x1,x2,y,y1,y2,color, radius;
 float t1,t2;
 word i,start;
 BITMAP bmp; 

 // load all files/bitmaps here
 load_bmp("rocket.bmp", &bmp);

 set_mode(0x13);
 VGA+=__djgpp_conventional_base;
 my_clock = (void *)my_clock + __djgpp_conventional_base;

 draw_bitmap(&bmp,150,10); 

 // plot_pixel_memory(100,100,1);

 // line_slow(5, 5, 10, 10, 5);

 // rect_fill(200, 50, 250, 50, 13);

 // circle_fast(100, 100, 40, 10);

 radius=rand()%90+1;
 x=rand()%(320-radius*2)+radius;
 y=rand()%(200-radius*2)+radius;
 color=12;
 circle_fill(x,y,radius,color);

 x1=rand()%320;
 y1=rand()%200;
 x2=rand()%320;
 y2=rand()%200;
 color=14;
 rect_fill(x1,y1,x2,y2,color);

 int num_vertices=3;
 int vertices[6]={5,0,
                  7,5,
                  1,4};
 polygon(3,vertices,15);
 

 while(getchar() != '\n');

 set_mode(0x03);

 return 0;
}
