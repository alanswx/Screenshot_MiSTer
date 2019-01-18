/*
Copyright 2005, 2006, 2007 Dennis van Weeren
Copyright 2008, 2009 Jakub Bednarski
Copyright 2012 Till Harbaum

This file is part of Minimig

Minimig is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Minimig is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

const char *version = "$VER:HPS" VDATE;

#define BASEADDR 536870912 


//gameboy
//#define width 160
//#define height 144
//genesis
//#define width 320
#define width 256
#define height 448
//#define width 448
//#define height 320
#define pixels 3

int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}

int main(int argc, char *argv[])
{
   
    void *handle;
    char *file = "/dev/mem";
    int ret;

    printf("WIDTH: %d\n",width);
    printf("WIDTH: %d\n",width*3);
    int new_width = roundUp(width*3,256);
    printf("NEW WIDTH: %d\n",new_width);
    printf("NWIDTH: %d\n",new_width/3);
    int nwidth = new_width/3;
    unsigned char buffer[new_width*height];

    handle = memtool_open(file, O_RDONLY);
    printf("\nhandle= %x\n",handle);
    ret = memtool_read(handle, BASEADDR, buffer, new_width*height, 1);
    //for (int c = 0 ; c < new_width*height;c++)
    //   ret = memtool_read(handle, BASEADDR+c, &(buffer[c]),1,1);
    printf("\nret = %d\n",ret);
    memtool_close(handle); 
    printf("\nclose\n");

    FILE *out = fopen("out.pbm","w");
	// Always pin main worker process to core #1 as core #0 is the
	// hardware interrupt handler in Linux.  This reduces idle latency
	// in the main loop by about 6-7x.
	printf("\nScreenshot code by alanswx\n\n");

	printf("Version %s\n\n", version + 5);

        //unsigned char *pixbuf = (unsigned char *)536870912;
        unsigned char *pixbuf = buffer;
        fprintf(out,"P3\n%d %d\n255\n",nwidth,height);
        for (int  y=0; y< height ; y++)
          for (int x = 0; x < nwidth ; x++)
          { 
            unsigned char r,g,b;
            r = *pixbuf++;
            g = *pixbuf++;
            b = *pixbuf++;
            //if (x>=width) continue;
            printf("%x %x %x\n",r,g,b);
            fprintf(out,"%d %d %d ",r,g,b);
            if (x==nwidth-1) 
            {
              fprintf(out,"\n");
              int dif = new_width - (nwidth*3);
              pixbuf=pixbuf+dif;
            }
          }

        fclose(out);

	return 0;
}
