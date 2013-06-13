#ifndef LCDSIM
#define LCDSIM
#include <math.h>

#define NUM_SEGMENTS 16
float lcdDisplayParamBottom[NUM_SEGMENTS][5] = 
{
  {0,0,0,0,0},
  {0,12, 4,1,M_PI/2},  //Top left |
  {4,8, 3,1,0},       //middle left -
  {0,4, 4,1,M_PI/2},  //bottom left |

  {0,0,0,0,0},
  {4,12,4,1,M_PI/4},   //bottom left /
  {4,4, 4,1,-M_PI/4},  //top left \ ;
  {7,0,6,1,0},   //Top -

  {0,0,0,0,0},
  {10,12, 4,1,-M_PI/4},  //bottom right \ ;
  {7,8, 7,1,M_PI/2},       //middle left -
  {10,4,4,1,M_PI/4},   //top right /

  {7,16, 6,1,0},       //bottom  -
  {14,12, 4,1,M_PI/2},  //Top right|
  {10,8, 3,1,0},       //middle right -
  {14,4, 4,1,M_PI/2},  //bottom right |

};

float lcdDisplayParamTop[NUM_SEGMENTS][5] = 
{
  {0,12, 4,1,M_PI/2},  //Top left |
  {4,8, 3,1,0},       //middle left -
  {0,4, 4,1,M_PI/2},  //bottom left |
  {0,0,0,0,0},

  {7,16, 6,1,0},       //bottom  -
  {4,12,4,1,M_PI/4},   //bottom left /
  {4,4, 4,1,-M_PI/4},  //top left \ ;
  {7,0,6,1,0},   //Top -

  {10,12, 4,1,-M_PI/4},  //bottom right \ ;
  {7,8, 7,1,M_PI/2},       //middle left -
  {10,4,4,1,M_PI/4},   //top right /
  {0,0,0,0,0},

  {14,12, 4,1,M_PI/2},  //Top right|
  {10,8, 3,1,0},       //middle right -
  {14,4, 4,1,M_PI/2},  //bottom right |
  {0,0,0,0,0},

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


void displaySegment(GtkWidget* widget, int x, int y, unsigned char top, unsigned short mask)
{
  float scale = 3;
  int i;
  for(i=0; i<NUM_SEGMENTS; i++)
  {
    char segmentOn = 0;
    if ( (1<<i) & mask )
      segmentOn = 1;
    if (top)
    {
      draw_segment( widget,
          lcdDisplayParamTop[i][0]+x, lcdDisplayParamTop[i][1]+y, 
          lcdDisplayParamTop[i][2], lcdDisplayParamTop[i][3]*scale, 
          lcdDisplayParamTop[i][4], 
          segmentOn);
    } else {
      draw_segment( widget,
          lcdDisplayParamBottom[i][0]+x, lcdDisplayParamBottom[i][1]+y, 
          lcdDisplayParamBottom[i][2], lcdDisplayParamBottom[i][3]*scale, 
          lcdDisplayParamBottom[i][4], 
          segmentOn);
    }

  }
}

#endif
