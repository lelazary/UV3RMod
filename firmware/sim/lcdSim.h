#ifndef LCDSIM
#define LCDSIM
#include <math.h>

#define NUM_SEGMENTS 16
float lcdDisplayParam[NUM_SEGMENTS][5] = 
{
  {0, 0,     3,1, 0}, //Left Top -
  {6.2, 0,   3,1, 0}, //Right Top -
  {9.40, 4.2,     4,1, M_PI/2}, // top left |
  {6.2, 8.2,   3,1, 0},   //middle left -
  {9.40, 4.2+8,     4,1, M_PI/2}, //bottom left |
  {6.2, 16.2,   3,1, 0}, //bottom left -
  {0, 16.2,   3,1, 0}, //bottom right -
  {-3.2, 4.2+8,     4,1, M_PI/2}, //bottom right |
  {-0.1, 8.2,   3,1, 0},  //middle right -
  {-3.2, 4.2,     4,1, M_PI/2}, //top right |
  {3, 4.2,     4,1, M_PI/2}, //top middle |
  {3, 4.2+8,     4,1, M_PI/2}, //bottom middle |
  {-0.1, 3.7,     4,1, -M_PI/3.0}, //top left  \ ;
  {6.1, 3.7+8,     4,1, -M_PI/3.0}, //bottom right 
  {0.1, 3.7+8,     4,1, M_PI/3.0}, //bottom right  /
  {6.1, 3.7,     4,1, M_PI/3.0} //top left  /
};

static void
draw_segment_dot( GdkGC *fg, GdkGC *bg, GdkWindow *window, int width, 
  int height, int x, int y ) 
{
  int wide, h_offset, margin, segwide;

  width = width << 1;
  wide = 0.4 * width;

  segwide = wide * 0.4;

  h_offset = (width - (wide + margin * 2)) / 2;

  gdk_draw_arc( (GdkDrawable *) window, fg, TRUE,
    x + segwide, y + (height - segwide), segwide, segwide, 0, 23040 );
}

static void
draw_segment( GtkWidget *widget,
    float x, float y, float length, float scale, float theta, char on)
{
  GdkPoint points[7];

  points[0].x = -length; points[0].y = 0;
  points[1].x = -(length-1); points[1].y = 1;
  points[2].x =  length-1; points[2].y = 1;
  points[3].x =  length; points[3].y = 0;
  points[4].x =  length-1; points[4].y = -1;
  points[5].x = -(length-1); points[5].y = -1;
  points[6].x = -length; points[6].y = 0;

  int i=0;
  for(i=0; i<7; i++)
  {
    float tx = points[i].x*cos(theta) + points[i].y*sin(theta);
    float ty = -points[i].x*sin(theta) + points[i].y*cos(theta);

    tx += x;
    ty += y;

    tx *= scale;
    ty *= scale;

    points[i].x = tx;
    points[i].y = ty;

  }

  GdkGC *fg = widget->style->black_gc;
  if (!on)
    fg = widget->style->bg_gc[GTK_STATE_NORMAL];

  gdk_draw_polygon( (GdkDrawable *) widget->window,
      fg, TRUE, &points[0], 7 );


}


void displaySegment(GtkWidget* widget, int x, int y, unsigned short mask)
{
  float scale = 3;
  int i;
  for(i=0; i<NUM_SEGMENTS; i++)
  {
    char segmentOn = 0;
    if ( (1<<i) & mask )
      segmentOn = 1;
    draw_segment( widget,
        lcdDisplayParam[i][0]+x, lcdDisplayParam[i][1]+y, 
        lcdDisplayParam[i][2], lcdDisplayParam[i][3]*scale, 
        lcdDisplayParam[i][4], 
        segmentOn);
  }

}

#endif
