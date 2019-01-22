/*
Copyright 2019 alanswx
with help from the MiSTer contributors including Grabulosaure 
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sched.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "fileaccess.h"

const char *version = "$VER:ScreenShot" VDATE;

#define BASEADDR 536870912 

unsigned char buffer[2048*3*1024];

int main(int argc, char *argv[])
{
   
    void *handle;
    char *file = "/dev/mem";
    int ret;
    
    int width,height,line,header;
    
    
    handle = memtool_open(file, O_RDONLY);
    
    ret = memtool_read(handle, BASEADDR, buffer, 128,4);
    if (buffer[0]!=1 || buffer[1]!=1) {
        memtool_close(handle);
        return -1;
    }
    
    header=buffer[2]<<8 | buffer[3];
    width =buffer[6]<<8 | buffer[7];
    height=buffer[8]<<8 | buffer[9];
    line  =buffer[10]<<8 | buffer[11];
    
    
    printf ("Image: Width=%i Height=%i  Line=%i  Header=%i\n",width,height,line,header);
/*
    printf (" 1: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\n",
            buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],
            buffer[8],buffer[9],buffer[10],buffer[11],buffer[12],buffer[13],buffer[14],buffer[15]);
    */

    ret = memtool_read(handle, BASEADDR, buffer, header + height*line, 4);
    memtool_close(handle);
    
    FILE *out = fopen("out.pbm","w");
	printf("\nScreenshot code by alanswx\n\n");
	printf("Version %s\n\n", version + 5);
    
    unsigned char *pixbuf;
    fprintf(out,"P3\n%d %d\n255\n",width,height);
    for (int  y=0; y< height ; y++) {
          pixbuf=&buffer[header + y*line];
          for (int x = 0; x < width ; x++) { 
            unsigned char r,g,b;
            r = *pixbuf++;
            g = *pixbuf++;
            b = *pixbuf++;
            fprintf(out,"%d %d %d ",r,g,b);
          }
          fprintf(out,"\n");
    }

    fclose(out);
    
    return 0;
}
