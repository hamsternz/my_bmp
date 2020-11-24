///////////////////////////////////////////
// my_bmp.c - Write BMP file
//
// Author: Mike Field <hamster@snap.net.nz>
///////////////////////////////////////////
#include <stdio.h>
#include "my_bmp.h"

static int calc_data_size(int width, int height) {
   return ((width+3)/4)*4*width;
}

static int write_16bit(FILE *f, unsigned int val) {
   if(putc(val>>0,  f) == EOF) return 0;
   if(putc(val>>8,  f) == EOF) return 0;
   return 1;
}

static int write_32bit(FILE *f, unsigned int val) {
   if(putc(val>>0,  f) == EOF) return 0;
   if(putc(val>>8,  f) == EOF) return 0;
   if(putc(val>>16, f) == EOF) return 0;
   if(putc(val>>24, f) == EOF) return 0;
   return 1;
}

static int write_bitmap_header(FILE *f, int width, int height) {
   unsigned header_size;
   unsigned total_size;
   unsigned int data_size = calc_data_size(width,height);

   header_size = 14+40;
   data_size = ((width+3)/4)*4*width;
   total_size = 14+40+data_size;

   // Header
   if(putc('B', f) == EOF) return 0;
   if(putc('M', f) == EOF) return 0;
   // Size
   if(!write_32bit(f,total_size)) return 0;

   // Reserved
   if(!write_32bit(f,0)) return 0;

   // Offset
   if(!write_32bit(f,header_size)) return 0;

   return 1;
}

static int write_dib_header(FILE *f, int width, int height) {
   unsigned int data_size = calc_data_size(width,height);
   unsigned int pixels_per_meter = 2834; 
   // Header Size
   if(!write_32bit(f,40)) return 0;

   // Width
   if(!write_32bit(f,width)) return 0;

   // Height
   if(!write_32bit(f,height)) return 0;

   // Colour planes
   if(!write_16bit(f,1)) return 0;

   // Bits per pixel
   if(!write_16bit(f,24)) return 0;

   // Compression
   if(!write_32bit(f,0)) return 0;

   // Image size
   if(!write_32bit(f,data_size)) return 0;
   
   // Horizontal pixels per meter
   if(!write_32bit(f,pixels_per_meter)) return 0;
   
   // Vertical pixels per meter
   if(!write_32bit(f,pixels_per_meter)) return 0;
   
   // Colours used
   if(!write_32bit(f,0)) return 0;

   // Important colours
   if(!write_32bit(f,0)) return 0;

   return 1;
}

static int write_pixel_data(FILE *f, unsigned char *data, int width, int height) {
   for(int i=0; i < height; i++) {
      for(int j = 0; j < width; j++) {
         if(putc(data[2],f)==EOF) return 0; 
         if(putc(data[1],f)==EOF) return 0; 
         if(putc(data[0],f)==EOF) return 0; 
         data += 3;
      }

      // Pad out to the nearest four-byte boundary
      int pad = (width*3)%4;
      while(pad % 4 != 0) {
         if(putc(0,f)==EOF) {
            fprintf(stderr,"Write error - padding\n");
            return 0; 
         }
         pad++;
      }
   }
   return 1;
}

int mybmp_write(unsigned char *data, int width, int height, char *filename) {
   FILE *f = fopen(filename,"wb");
   if(f == NULL) {
     return 0;
   }

   if(!write_bitmap_header(f, width, height)) 
     return 0;

   if(!write_dib_header(f, width, height))
     return 0;

   if(!write_pixel_data(f, data, width, height))
     return 0;

   fclose(f);
   return 1; 
}
