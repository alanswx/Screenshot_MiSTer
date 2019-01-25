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

#include "memtool/fileaccess.h"

#include "mister_scalar.h"

const char *version = "$VER:ScreenShot" VDATE;

#define BASEADDR 536870912 

unsigned char buffer[2048*3*1024];

void mister_scalar_free(mister_scalar *);


int main(int argc, char *argv[])
{
    mister_scalar *ms=mister_scalar_init();
    if (ms==NULL)
    {
	    printf("some problem with the mister scalar, maybe this core doesn't support it\n");
	    exit(0);
    } 
    FILE *out = fopen("out.pbm","w");
	printf("\nScreenshot code by alanswx\n\n");
	printf("Version %s\n\n", version + 5);
   
    unsigned char *outputbuf = calloc(ms->width*ms->height*3,1);	
	printf("before read\n");
    mister_scalar_read(ms,outputbuf);
	printf("after read\n");
    unsigned char *pixbuf=outputbuf;
    fprintf(out,"P3\n%d %d\n255\n",ms->width,ms->height);
    for (int  y=0; y< ms->height ; y++) {
          for (int x = 0; x < ms->width ; x++) { 
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
