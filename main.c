#include <stdio.h>
#include <memory.h>
#include "my_bmp.h"

#define WIDTH  800
#define HEIGHT 600

unsigned char data[HEIGHT][WIDTH][3];

void colourbars(void) {
   int i;
   for(i = 0; i < WIDTH; i++) {
     switch(7*i/WIDTH) {
        case 0:  // White
           data[0][i][0] = 192;
           data[0][i][1] = 192;
           data[0][i][2] = 192;
           break;
        case 1: // Yelow
           data[0][i][0] = 192;
           data[0][i][1] = 192;
           data[0][i][2] =   0;
           break;
        case 2: // Cyan
           data[0][i][0] =   0;
           data[0][i][1] = 192;
           data[0][i][2] = 192;
           break;
        case 3: // Green
           data[0][i][0] =   0;
           data[0][i][1] = 192;
           data[0][i][2] =   0;
           break;
        case 4: // Magenta
           data[0][i][0] = 192;
           data[0][i][1] =   0;
           data[0][i][2] = 192;
           break;
        case 5: // Red
           data[0][i][0] = 192;
           data[0][i][1] =   0;
           data[0][i][2] =   0;
           break;
        default: // Blue
           data[0][i][0] =   0;
           data[0][i][1] =   0;
           data[0][i][2] = 192;
           break;
     }
   }
#if 1
   for(i = 1; i < HEIGHT; i++) {
      memcpy(data[i],data[0], WIDTH*3);
   }
#endif
}
int main(int argc, char *argv[]) {
   colourbars();
   if(mybmp_write(data[0][0], WIDTH, HEIGHT, "output.bmp")) {
      printf("File write OK\n");
   } else {
      printf("File write ERROR\n");
   }
}
