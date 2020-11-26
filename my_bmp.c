///////////////////////////////////////////
// my_bmp.c - Write BMP file
//
// Author: Mike Field <hamster@snap.net.nz>
///////////////////////////////////////////
#include <stdio.h>
#include <malloc.h>
#include "my_bmp.h"

static int calc_data_size(int width, int height) {
   return ((width+3)/4)*4*width;
}

static int read_16bit(FILE *f, unsigned int *val) {
   unsigned int v;
   int c;

   if((c = getc(f)) == EOF) return 0;
   v = c;

   if((c = getc(f)) == EOF) return 0;
   v = v | (c<<8);

   *val = v; 
   return 1;
}

static int read_32bit(FILE *f, unsigned int *val) {
   unsigned int v;
   int c;

   if((c = getc(f)) == EOF) return 0;
   v = c;

   if((c = getc(f)) == EOF) return 0;
   v = v | (c << 8);

   if((c = getc(f)) == EOF) return 0;
   v = v | (c << 16);

   if((c = getc(f)) == EOF) return 0;
   v = v | (c << 24);

   *val = v; 
   return 1;
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

static int read_bitmap_header(FILE *f, size_t *data_offset) {
   unsigned int total_size;
   unsigned int ignore;
   unsigned int o;

   // Header
   if(getc(f) != 'B') {
      printf("Bad sig 1\n");
      return 0;
   }
   if(getc(f) != 'M') {
      printf("Bad sig 2\n");
      return 0;
   }
   // Size
   if(!read_32bit(f,&total_size)) {
     printf("bad size\n");
     return 0;
   }

   // Reserved
   if(!read_32bit(f,&ignore)) {
     printf("bad size\n");
     return 0;
   }

   // Offset
   if(!read_32bit(f,&o)) {
     printf("bad offset size\n");
     return 0;
   }
   *data_offset = 0;
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

static int read_dib_header(FILE *f, int *width, int *height) {
   unsigned int header_size;
   unsigned int data_size;
   unsigned int pixels_per_meter_h;
   unsigned int pixels_per_meter_v;
   unsigned int compression;
   unsigned int colour_planes;
   unsigned int colours_used;
   unsigned int important_colours;
   unsigned int bits_per_pixel;
   unsigned int w, h;
   // Header Size
   if(!read_32bit(f,&header_size)) return 0;

   // Width
   if(!read_32bit(f,&w)) return 0;

   // Height
   if(!read_32bit(f,&h)) return 0;

   // Colour planes
   if(!read_16bit(f,&colour_planes)) return 0;

   // Bits per pixel
   if(!read_16bit(f,&bits_per_pixel)) return 0;

   // Compression
   if(!read_32bit(f,&compression)) return 0;

   // Image size
   if(!read_32bit(f,&data_size)) return 0;
   
   // Horizontal pixels per meter
   if(!read_32bit(f,&pixels_per_meter_h)) return 0;
   
   // Vertical pixels per meter
   if(!read_32bit(f,&pixels_per_meter_v)) return 0;
   
   // Colours used
   if(!read_32bit(f,&colours_used)) return 0;

   // Important colours
   if(!read_32bit(f,&important_colours)) return 0;

   if(bits_per_pixel != 24) {
      printf("Unsupported pixel depth %i\n", bits_per_pixel);
      return 0;
   }

   if(compression != 0) {
      printf("Unsupported compression\n");
      return 0;
   }

   if(colour_planes != 1) {
      printf("Unsupported colour planes\n");
      return 0;
   }

   *width = w;
   *height = h;
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

static int read_pixel_data(FILE *f, size_t offset, unsigned char *data, int width, int height) {
   if(fseek(f,offset, SEEK_SET) == -1) {
      return 0;
   }

   for(int i=0; i < height; i++) {
      for(int j = 0; j < width; j++) {
         int r,g,b;
         b = getc(f);
         if(b == EOF) return 0;  
         g = getc(f);
         if(g == EOF) return 0;  
         r = getc(f);
         if(r == EOF) return 0;  
         data[0] = r;
         data[1] = g;
         data[2] = b;
         data += 3;
      }
      int pad = (width*3)%4;
      while(pad % 4 != 0) {
         if(getc(f)==EOF) {
            fprintf(stderr,"Read error - padding\n");
            return 0; 
         }
         pad++;
      }
   }
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

int mybmp_read(unsigned char **data, int *width, int *height, char *filename) {
   int w,h;
   size_t o = 54;
   unsigned char *d;

   FILE *f = fopen(filename,"rb");
   if(f == NULL) {
     return 0;
   }

   if(!read_bitmap_header(f, &o)) {
     printf("Bad header\n");
     fclose(f);
     return 0;
   }

   if(!read_dib_header(f, &w, &h)) {
     printf("Bad DIB header\n");
     fclose(f);
     return 0;
   }

   d = malloc(w*h*3);
   if(d == NULL) {
     printf("Out of mem\n");
     fclose(f);
     return 0;
   }

   if(!read_pixel_data(f, o, d, w, h)) {
     printf("Bad data\n");
     free(d);
     fclose(f);
     return 0;
   }

   fclose(f);

   // All good, so assign values
   *data = d;
   *width = w;
   *height = h;
   return 1; 
}

int mybmp_write(unsigned char *data, int width, int height, char *filename) {
   FILE *f = fopen(filename,"wb");
   if(f == NULL) {
     return 0;
   }

   if(!write_bitmap_header(f, width, height)) {
     fclose(f);
     return 0;
   }

   if(!write_dib_header(f, width, height)) {
     fclose(f);
     return 0;
   }
   if(!write_pixel_data(f, data, width, height)) {
     fclose(f);
     return 0;
   }

   fclose(f);
   return 1; 
}
