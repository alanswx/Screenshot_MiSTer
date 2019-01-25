/*
Copyright 2019 alanswx
with help from the MiSTer contributors including Grabulosaure 
*/


typedef struct {
   int header;
   int width;
   int height;
   int line;

   void *handle;
} mister_scalar;


#define MISTER_SCALAR_BASEADDR     536870912 
#define MISTER_SCALAR_BUFFERSIZE   2048*3*1024


mister_scalar *mister_scalar_init();
int mister_scalar_read(mister_scalar *,unsigned char *buffer);
void mister_scalar_free(mister_scalar *);
