#include "led-matrix.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

static void DrawOnCanvas(Canvas *canvas) {
    canvas->SetPixel(center_x + dot_x, center_y + dot_y,
                     255, 0, 0);
    usleep(1 * 1000);  // wait a little to slow down things.
}

int main(int argc, char *argv[]) {
  /*
   * Set up GPIO pins. This fails when not running as root.
   */
  GPIO io;
  if (!io.Init())
    return 1;

  /*
   * Set up the RGBMatrix. It implements a 'Canvas' interface.
   */
  int rows     = 32;    // A 32x32 display. Use 16 when this is a 16x32 display.
  int chain    =  6;    // Number of boards chained together.
  int parallel =  1;    // Number of chains in parallel (1..3). > 1 for plus or Pi2
  Canvas *canvas = new RGBMatrix(&io, rows, chain, parallel);

  DrawOnCanvas(canvas);    // Using the canvas.

  canvas->Clear();
  delete canvas;

  return 0;
}
