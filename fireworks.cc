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

#define NUM 40
#define WIDTH  3
#define HEIGHT 2

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
  bool draw;
}star;


void render(Canvas* c, star* s)
{
  if(s->draw){
    leds[(int)s->posX][(int)s->posY][0]= s->r;
    leds[(int)s->posX][(int)s->posY][1]= s->g;
    leds[(int)s->posX][(int)s->posY][2]= s->b;
  }
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
     int rr=55 + rand()%200;
     int rb=55 + rand()%200;
     int rg=55 + rand()%200;
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
     s->draw=true;
     double speed = ((double)(rand()%10000))/20000.0;
     double dir = ((double)(rand()%10000))*M_PI/(5000.0);
     s->speedX=speed*cos(dir);
     s->speedY=speed*sin(dir);
     }
     for(int b = 0; b< 100;b++){
        for(int i = 0; i < NUM; i++){
            star* s = &(stars[i]);
            s->posX += s->speedX;
            s->posY += s->speedY;
            s->r *= 0.99;
            s->g *= 0.99;
            s->b *= 0.99;
	    if(s->posX <= 0){s->draw=false;}
            if(s->posY <= 0){s->draw=false;}
            if(s->posX >= WIDTH*32){s->draw=false;}
            if(s->posY >= HEIGHT*32){s->draw=false;}
            render(canvas,s);
        }
      draw(canvas);
      usleep(5000);
    }

    for(int b = 0; b < 40; b++){
      draw(canvas);
      usleep(5000);
    }
    usleep(200000);
  }

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}
