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

#include "mister_scalar.h"

#include "memtool/fileaccess.h"


mister_scalar * mister_scalar_init()
{
    mister_scalar *ms = calloc(sizeof(mister_scalar),1);
    char *file = "/dev/mem";
    unsigned char buffer[MISTER_SCALAR_BUFFERSIZE];
    ms->handle = memtool_open(file, O_RDONLY);
    
    memtool_read(ms->handle, MISTER_SCALAR_BASEADDR, buffer, 128,4);
    printf (" 1: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\n",
            buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],
            buffer[8],buffer[9],buffer[10],buffer[11],buffer[12],buffer[13],buffer[14],buffer[15]);
    if (buffer[0]!=1 || buffer[1]!=1) {
        printf("problem\n");
        mister_scalar_free(ms);
        return NULL;
    }
    
    ms->header=buffer[2]<<8 | buffer[3];
    ms->width =buffer[6]<<8 | buffer[7];
    ms->height=buffer[8]<<8 | buffer[9];
    ms->line  =buffer[10]<<8 | buffer[11];
   
    printf ("Image: Width=%i Height=%i  Line=%i  Header=%i\n",ms->width,ms->height,ms->line,ms->header);
   /*
    printf (" 1: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\n",
            buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],
            buffer[8],buffer[9],buffer[10],buffer[11],buffer[12],buffer[13],buffer[14],buffer[15]);
    */

   return ms; 

}
void mister_scalar_free(mister_scalar *ms)
{
   memtool_close(ms->handle);
   free(ms);
}


int mister_scalar_read(mister_scalar *ms,unsigned char *gbuf)
{
    unsigned char buffer[MISTER_SCALAR_BUFFERSIZE];
    
    printf("a\n");    
    memtool_read(ms->handle, MISTER_SCALAR_BASEADDR, buffer, ms->header + ms->height*ms->line, 4);
    printf("b\n");    
   
    // do this slow way for now.. 
    unsigned char *pixbuf;
    unsigned char *outbuf;
    printf("c\n");    
    for (int  y=0; y< ms->height ; y++) {
          pixbuf=&buffer[ms->header + y*ms->line];
          outbuf=&gbuf[y*(ms->width*3)];
          for (int x = 0; x < ms->width ; x++) { 
            *outbuf++ = *pixbuf++;
            *outbuf++ = *pixbuf++;
            *outbuf++ = *pixbuf++;
          }
    }
    
    return 0;
}
