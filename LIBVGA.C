#include "libvga.h"

byte *VGA = (byte *)0xA0000;
word *my_clock = (word *)0x046C;

void set_mode(int mode)
{
 int i;
 for(i=0;i<1024;i++)                 /* create the sin(arccos(x)) table. */
 {    
  SIN_ACOS[i]=sin(acos((float)i/1024))*0x10000L;
 }

 union REGS regs;

 regs.h.ah = 0x00;
 regs.h.al = mode;
 int86(0x10,&regs,&regs);
}

void plot_pixel_bios(int x, int y, byte color)
{
 union REGS regs;

 regs.h.ah = 0x0C;
 regs.h.al = color;
 regs.x.cx = x;
 regs.x.dx = y;
 int86(0x10,&regs,&regs);
}

void plot_pixel_memory(int x, int y, byte color)
{
 VGA[(y<<8)+(y<<6)+x]=color;
}

void line_slow(int x1, int y1, int x2, int y2, byte color)
{
 int dx,dy,sdx,sdy,px,py;
 int dxabs,dyabs,i;
 float slope;

 dx=x2-x1;      // horizontal distance of line
 dy=y2-y1;      // vertical distance of line
 dxabs=abs(dx);
 dyabs=abs(dy);
 sdx=sgn(dx);
 sdy=sgn(dy);
 if (dxabs>=dyabs)      // line more horizontal than verticle
 {
  slope=(float)dy / (float)dx;
  for(i=0; i!=dx; i+=sdx)
  {
   px=slope*i+x1;
   py=i+y1;
   plot_pixel_memory(px,py,color);
  }
 }
 else  // line is more verticle than horizontal
 {
  slope=(float)dx / (float)dy;
  for(i=0; i!=dy; i+=sdy)
  {
   px=slope*i+x1;
   py=i+y1;
   plot_pixel_memory(px,py,color);
  }
 }
}

void line_fast(int x1, int y1, int x2, int y2, byte color)
{
 int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

 dx=x2-x1;      // horizontal distance of line
 dy=y2-y1;      // verticle distance of line
 dxabs=abs(dx);
 dyabs=abs(dy);
 sdx=sgn(dx);
 sdy=sgn(dy);
 x=dyabs>>1;
 y=dxabs>>1;
 px=x1;
 py=y1;

 VGA[(py<<8)+(py<<6)+px]=color;

 if(dxabs>=dyabs)
 {
  for(i=0;i<dxabs;i++)
  {
   y+=dyabs;
   if(y>=dxabs)
   {
    y-=dxabs;
    py+=sdy;
   }
   px+=sdx;
   plot_pixel_memory(px,py,color);
  }
 }
 else
 {
  for(i=0;i<dyabs;i++)
  {
   x+=dxabs;
   if(x>=dyabs)
   {
    x-=dyabs;
    px+=sdx;
   }
   py+=sdy;
   plot_pixel_memory(px,py,color);
  }
 }
}

void polygon(int num_vertices, int *vertices, byte color)
{
 int i;

 for(i=0; i<num_vertices-1; i++)
 {
  line_fast(vertices[(i<<1)+0],
            vertices[(i<<1)+1],
            vertices[(i<<1)+2],
            vertices[(i<<1)+3],
            color);
 }
 line_fast(vertices[0],
           vertices[1],
           vertices[(num_vertices<<1)-2],
           vertices[(num_vertices<<1)-1],
           color);
}

void rect_slow(int left, int top, int right, int bottom, byte color)
{
 line_fast(left,top,right,top,color);
 line_fast(left,top,left,bottom,color);
 line_fast(right,top,right,bottom,color);
 line_fast(left,bottom,right,bottom,color);
}

void rect_fast(int left, int top, int right, int bottom, byte color)
{
 word top_offset,bottom_offset,i,temp;

 if(top>bottom)
 {
  temp=top;
  top=bottom;
  bottom=temp;
 }

 if(left>right)
 {
  temp=left;
  left=right;
  right=temp;
 }

 top_offset=(top<<8)+(top<<6);
 bottom_offset=(bottom<<8)+(bottom<<6);

 for(i=left;i<=right;i++)
 {
  VGA[top_offset+i]=color;
  VGA[bottom_offset+i]=color;
 }

 for(i=top_offset;i<=bottom_offset;i+=320)
 {
  VGA[left+i]=color;
  VGA[right+i]=color;
 }
}

void rect_fill(int left, int top, int right, int bottom, byte color)
{
 word top_offset,bottom_offset,i,temp,width;

 if (top>bottom)
 {
  temp=top;
  top=bottom;
  bottom=temp;
 }
 if (left>right)
 {
  temp=left;
  left=right;
  right=temp;
 }

 top_offset=(top<<8)+(top<<6)+left;
 bottom_offset=(bottom<<8)+(bottom<<6)+left;
 width=right-left+1;

 for(i=top_offset;i<=bottom_offset;i+=320)
 {
  memset(&VGA[i],color,width);
 }
}

void circle_slow(int x, int y, int radius, byte color)
{
 float n=0,invradius=1/(float)radius;
 int dx=0, dy=radius-1;
 word dxoffset,dyoffset,offset=(y<<8)+(y<<6)+x;

 while (dx<=dy)
 {
  dxoffset = (dx<<8) + (dx<<6);
  dyoffset = (dy<<8) + (dy<<6);
  VGA[offset+dy-dxoffset] = color;      // octant 0
  VGA[offset+dx-dyoffset] = color;      // octant 1
  VGA[offset-dx-dyoffset] = color;      // octant 2
  VGA[offset-dy-dxoffset] = color;      // octant 3
  VGA[offset-dy+dxoffset] = color;      // octant 4
  VGA[offset-dx+dyoffset] = color;      // octant 5
  VGA[offset+dx+dyoffset] = color;      // octant 6
  VGA[offset+dy+dxoffset] = color;      // octant 7
  dx++;
  n+=invradius;
  dy=radius * sin(acos(n));
 }
}

void circle_fast(int x, int y, int radius, byte color)
{
 fixed16_16 n=0,invradius=(1/(float)radius)*0x10000L;
 int dx=0,dy=radius-1;
 word dxoffset,dyoffset,offset = (y<<8)+(y<<6)+x;

 while (dx<=dy)
 {
  dxoffset = (dx<<8) + (dx<<6);
  dyoffset = (dy<<8) + (dy<<6);
  VGA[offset+dy-dxoffset] = color;      // octant 0
  VGA[offset+dx-dyoffset] = color;      // octant 1
  VGA[offset-dx-dyoffset] = color;      // octant 2
  VGA[offset-dy-dxoffset] = color;      // octant 3
  VGA[offset-dy+dxoffset] = color;      // octant 4
  VGA[offset-dx+dyoffset] = color;      // octant 5
  VGA[offset+dx+dyoffset] = color;      // octant 6
  VGA[offset+dy+dxoffset] = color;      // octant 7
  dx++;
  n+=invradius;
  dy = (int)((radius * SIN_ACOS[(int)(n>>6)]) >> 16);
 }
}

void circle_fill(int x, int y, int radius, byte color)
{
 fixed16_16 n=0,invradius=(1/(float)radius)*0x10000L;
 int dx=0,dy=radius-1,i;
 word dxoffset,dyoffset,offset = (y<<8)+(y<<6)+x;

 while (dx<=dy)
 {
    dxoffset = (dx<<8)+(dx<<6);
    dyoffset = (dy<<8)+(dy<<6);
    for(i=dy;i>=dx;i--,dyoffset-=320)
    {
      VGA[offset+i -dxoffset] = color;  // octant 0
      VGA[offset+dx-dyoffset] = color;  // octant 1
      VGA[offset-dx-dyoffset] = color;  // octant 2
      VGA[offset-i -dxoffset] = color;  // octant 3
      VGA[offset-i +dxoffset] = color;  // octant 4
      VGA[offset-dx+dyoffset] = color;  // octant 5
      VGA[offset+dx+dyoffset] = color;  // octant 6
      VGA[offset+i +dxoffset] = color;  // octant 7
    }
    dx++;
    n+=invradius;
    dy = (int)((radius * SIN_ACOS[(int)(n>>6)]) >> 16);
 }
}

// bitmap
void fskip(FILE *fp, int num_bytes)
{
 int i;
 for (i=0; i<num_bytes; i++)
     fgetc(fp);
}

void load_bmp(char *file, BITMAP *b)
{
 FILE *fp;
 long index;
 word num_colors;
 int x;

 // open the file
 if ((fp = fopen(file,"rb")) == NULL)
 {
  printf("File opening file %s.\n", file);
  exit(1);
 }

 // check to see if the file is valid
 if (fgetc(fp)!='B' || fgetc(fp)!='M')
 {
  fclose(fp);
  printf("%s is not a valid bitmap file.\n", file);
  exit(1);
 }

 // read width and height of the image, and colors used
 fskip(fp,16);
 fread(&b->width, sizeof(word), 1, fp);
 fskip(fp,2);
 fread(&b->height, sizeof(word), 1, fp);
 fskip(fp,22);
 fread(&num_colors, sizeof(word), 1, fp);
 fskip(fp,6);

 // assume we are working with an 8bit file
 if (num_colors==0)
    num_colors=256;

 // try to alloc memory
 if ((b->data = (byte *) malloc((word)(b->width*b->height))) == NULL)
 {
  fclose(fp);
  printf("Error allocating memory for file %s.\n", file);
  exit(1);
 }

 // ignore pallet info
 fskip(fp,num_colors*4);

 // read bitmap
 for(index=(b->height-1)*b->width;index>=0;index-=b->width)
   for(x=0;x<b->width;x++)
     b->data[(word)index+x]=(byte)fgetc(fp);

 fclose(fp);
}

void draw_bitmap(BITMAP *bmp,int x,int y)
{
 int j;
 word screen_offset = (y<<8)+(y<<6)+x;
 word bitmap_offset = 0;

 for(j=0;j<bmp->height;j++)
 {
  memcpy(&VGA[screen_offset],&bmp->data[bitmap_offset],bmp->width);

  bitmap_offset+=bmp->width;
  screen_offset+=320;
 }
}

void draw_transparent_bitmap(BITMAP *bmp, int x, int y)
{
 int i,j;
 word screen_offset = (y<<8)+(y<<6);
 word bitmap_offset = 0;
 byte data;

 for(j=0;j<bmp->height;j++)
 {
  for(i=0;i<bmp->width;i++,bitmap_offset++)
  {
   data = bmp->data[bitmap_offset];
   if (data) VGA[screen_offset+x+i] = data;
  }
  screen_offset+=320;
 }
} 

void wait(int ticks)
{
 word start;

 start=*my_clock;

 while (*my_clock-start<ticks)
 {
  *my_clock=*my_clock;
 }
} 
