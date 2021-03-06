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

#include <sys/types.h>
#include <sys/mman.h>
#include <err.h>



#include "mister_scalar.h"



mister_scalar * mister_scalar_init()
{
    mister_scalar *ms = calloc(sizeof(mister_scalar),1);
    char *file = "/dev/mem";
    int	 pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize==0) pagesize=4096;
    int offset = MISTER_SCALAR_BASEADDR;
    int	map_start = offset & ~(pagesize - 1);
    ms->map_off = offset - map_start;
    ms->num_bytes=MISTER_SCALAR_BUFFERSIZE;
    //printf("map_start = %d map_off=%d offset=%d\n",map_start,ms->map_off,offset);

    unsigned char *buffer;
    ms->fd=open(file, O_RDONLY, S_IRUSR | S_IWUSR);
    ms->map=mmap(NULL, ms->num_bytes+ms->map_off,PROT_READ, MAP_SHARED, ms->fd, map_start);
    if (ms->map==MAP_FAILED)
    {
        fprintf(stderr,"problem MAP_FAILED\n");
        mister_scalar_free(ms);
        return NULL;
    }
    buffer = (unsigned char *)(ms->map+ms->map_off);
    fprintf (stderr," 1: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\n",
            buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],
            buffer[8],buffer[9],buffer[10],buffer[11],buffer[12],buffer[13],buffer[14],buffer[15]);
    if (buffer[0]!=1 || buffer[1]!=1) {
        fprintf(stderr,"problem\n");
        mister_scalar_free(ms);
        return NULL;
    }
    
    ms->header=buffer[2]<<8 | buffer[3];
    ms->width =buffer[6]<<8 | buffer[7];
    ms->height=buffer[8]<<8 | buffer[9];
    ms->line  =buffer[10]<<8 | buffer[11];
   
    fprintf (stderr,"Image: Width=%i Height=%i  Line=%i  Header=%i\n",ms->width,ms->height,ms->line,ms->header);
   /*
    printf (" 1: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\n",
            buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],
            buffer[8],buffer[9],buffer[10],buffer[11],buffer[12],buffer[13],buffer[14],buffer[15]);
    */

   return ms; 

}
void mister_scalar_free(mister_scalar *ms)
{
   munmap(ms->map,ms->num_bytes+ms->map_off);
   close(ms->fd);
   free(ms);
}

int mister_scalar_read_yuv(mister_scalar *ms,int lineY,unsigned char *bufY, int lineU, unsigned char *bufU, int lineV, unsigned char *bufV) {
    unsigned char *buffer;
    buffer = (unsigned char *)(ms->map+ms->map_off);

    // do this slow way for now.. 
    unsigned char *pixbuf;
    unsigned char *outbufy;
    unsigned char *outbufU;
    unsigned char *outbufV;
    for (int  y=0; y< ms->height ; y++) {
          pixbuf=&buffer[ms->header + y*ms->line];
          outbufy=&bufY[y*(lineY)];
          outbufU=&bufU[y*(lineU)];
          outbufV=&bufV[y*(lineV)];
          for (int x = 0; x < ms->width ; x++) { 
		 int R,G,B;
            R = *pixbuf++;
            G = *pixbuf++;
            B = *pixbuf++;
            int Y  =      (0.257 * R) + (0.504 * G) + (0.098 * B) + 16;
            int  U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128;
            int V =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128;

            *outbufy++ = Y;
            *outbufU++ = U;
            *outbufV++ = V;
          }
    }
    
    return 0;

}

int mister_scalar_read(mister_scalar *ms,unsigned char *gbuf)
{
    unsigned char *buffer;
    buffer = (unsigned char *)(ms->map+ms->map_off);
   
    // do this slow way for now..  - could use a memcpy?
    unsigned char *pixbuf;
    unsigned char *outbuf;
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
