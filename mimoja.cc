#include "led-matrix.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;
using namespace std;

#define NUM 30

#define WIDTH  3
#define HEIGHT_PANELS 1
#define CHAINS 2
#define HEIGHT HEIGHT_PANELS*CHAINS

static int leds[WIDTH*32][HEIGHT*32][3];

typedef struct _star
{
  double posX;
  double posY;
  double speedX;
  double speedY;
  int r;
  int g;
  int b;
}star;


void render(Canvas* c, star* s)
{
    leds[(int)s->posX][(int)s->posY][0]= s->r;
    leds[(int)s->posX][(int)s->posY][1]= s->g;
    leds[(int)s->posX][(int)s->posY][2]= s->b;
}

void draw(Canvas* canvas)
{
     for(int x = 0; x < WIDTH*32; x++){
        for(int y = 0; y < HEIGHT*32; y++){
          int* colors = leds[x][y];
          leds[x][y][0] *= 0.95;
          leds[x][y][1] *= 0.95;
          leds[x][y][2] *= 0.95;
          canvas->SetPixel(x,y,colors[0],colors[1],colors[2]);
         }
      }
}

int main(int argc, char *argv[]) {

  star stars[NUM];
  /*
   * Set up the RGBMatrix. It implements a 'Canvas' interface.
   */
  RGBMatrix::Options options;
  options.rows = 32;    // A 32x32 display. Use 16 when this is a 16x32 display.
  options.chain_length = WIDTH;  // Number of boards chained together.
  options.parallel = HEIGHT;      // Number of chains in parallel (1..3).
  Canvas *canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &options);

  while(1)
  {
obligatoryLabel:
     int rr=rand()%255;
     int rb=rand()%255;
     int rg=rand()%255;
     int rx=rand() % (WIDTH*32);
     int ry=rand() % (HEIGHT*32);
     for(int i = 0; i < NUM; i++)
     {
     star* s = &(stars[i]);
     s->posX = rx;
     s->posY = ry;
     s->r=rr;
     s->g=rb;
     s->b=rg;
     s->speedX=((rand() % 200)-100) / 255.0;
     s->speedY=((rand() % 200)-100) / 255.0;
     }
     for(int b = 0; b< 100;b++){
        for(int i = 0; i < NUM; i++){
            star* s = &(stars[i]);
            s->posX += s->speedX;
            s->posY += s->speedY;
            s->r *= 0.9995;
            s->g *= 0.9995;
            s->b *= 0.9995;
	    if(s->posX <= 0){s->posX=0; s->speedX*=-1;}
            if(s->posY <= 0){s->posY=0; s->speedY*=-1;}
            if(s->posX >= WIDTH*32){s->posX=WIDTH*32-1; s->speedX*=-1;}
            if(s->posY >= HEIGHT*32){s->posY=HEIGHT*32-1; s->speedY*=-1;}
            render(canvas,s);
            if(s->r == 0 && s->g == 0 && s->b == 0){goto obligatoryLabel;}
        }
      draw(canvas);
      usleep(5000);
    }

    for(int b = 0; b < 20; b++){
	draw(canvas);
   }
  }

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}
