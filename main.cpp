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

#include "lib/imlib2/Imlib2.h"

#include "scaler.h"

const char *version = "$VER:ScreenShot" VDATE;

#define BASEADDR 536870912 

unsigned char buffer[2048*3*1024];

void mister_scaler_free(mister_scaler *);


int main(int argc, char *argv[])
{
    char filename[4096];
    strcpy(filename,"MiSTer_screenshot.png");
    if (argc > 1) 
    {
        fprintf(stderr,"output name: %s\n", argv[1]);
	strcpy(filename,argv[1]);
    }

    mister_scaler *ms=mister_scaler_init();
    if (ms==NULL)
    {
	    fprintf(stderr,"some problem with the mister scaler, maybe this core doesn't support it\n");
	    exit(1);
    } 
    fprintf(stderr,"\nScreenshot code by alanswx\n\n");
    fprintf(stderr,"Version %s\n\n", version + 5);
   
    unsigned char *outputbuf = (unsigned char*)calloc(ms->width*ms->height*4,1);	
    mister_scaler_read_32(ms,outputbuf);
    Imlib_Image im = imlib_create_image_using_data(ms->width,ms->height,(unsigned int *)outputbuf);
    imlib_context_set_image(im);
    if (!strcmp("-",filename))
    {
	fprintf(stderr,"not implemented\n");
    }
    else
	imlib_save_image(filename);
    //free(outputbuf);
    mister_scaler_free(ms); 

    return 0;
}
