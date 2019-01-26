/*
 * Copyright (c) 2001 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * video encoding with libavcodec API example
 *
 * @example encode_video.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

#include "../mister_scalar.h"

//const char *version = "$VER:ScreenShot" VDATE;


static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   FILE *outfile)
{
    int ret;

    /* send the frame to the encoder */
    if (frame)
        printf("Send frame %3"PRId64"\n", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

#define DEBUG 0

int mister_scalar_read_frame(mister_scalar *ms,AVFrame *frame)
{
    unsigned char buffer[MISTER_SCALAR_BUFFERSIZE];

    memtool_read(ms->handle, MISTER_SCALAR_BASEADDR, buffer, ms->header + ms->height*ms->line, 4);
    ms->header=buffer[2]<<8 | buffer[3];

    #if DEBUG
    printf(" header5: %d\n",buffer[5]);
    int interlace = buffer[5]&0x01;
    int field= buffer[5]>>1&0x01;
    int hd= buffer[5]>>2&0x01;
    int vd= buffer[5]>>3&0x01;
    int tb= buffer[5]>>4& 0x01;
    int fc= (buffer[5]>>5) &0x07;
    printf(" interlace: %d field %d hd %d vd %d tb %d fc %d\n",interlace,field,hd,vd,tb,fc);
    printf(" header: %x \n",ms->header);
    printf (" 1: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\n",
            buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7],
            buffer[8],buffer[9],buffer[10],buffer[11],buffer[12],buffer[13],buffer[14],buffer[15]);
    #endif

    // do this slow way for now.. 
    unsigned char *pixbuf;
    unsigned char *pixY;
    unsigned char *pixU;
    unsigned char *pixV;
    for (int  y=0; y< ms->height ; y++) {
          pixbuf=&buffer[ms->header + y*ms->line];
          pixY=&frame->data[0][y * frame->linesize[0]];
          pixU=&frame->data[1][y/2 * frame->linesize[1]];
          pixV=&frame->data[2][y/2 * frame->linesize[2]];
          for (int x = 0; x < ms->width ; x++) {
                 int R,G,B;
                 R = *pixbuf++;
                 G = *pixbuf++;
                 B = *pixbuf++;
                 int Y  =      (0.257 * R) + (0.504 * G) + (0.098 * B) + 16;
		 *pixY++=Y;
		 if (!(x%2)) {
                   int  U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128;
                   int V =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128;
		   *pixU++=U;
		   *pixV++=V;
		 }
                 //frame->data[0][y * frame->linesize[0] + x] = Y;
                 //frame->data[1][y/2 * frame->linesize[1] + x/2] = U;
                 //frame->data[2][y/2 * frame->linesize[2] + x/2] = V;
          }
    }

    return 0;
}

int main(int argc, char **argv)
{
    const char *filename, *codec_name;
    const AVCodec *codec;
    AVCodecContext *c= NULL;
    int i, ret, x, y;
    FILE *f;
    AVFrame *frame;
    AVPacket *pkt;
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    int seconds=10;

    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <output file> <codec name>\n", argv[0]);
        exit(0);
    }
    filename = argv[1];
    codec_name = argv[2];

    av_register_all();
    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder_by_name(codec_name);
    if (!codec) {
        fprintf(stderr, "Codec '%s' not found\n", codec_name);
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);


    mister_scalar *ms=mister_scalar_init();
    if (ms==NULL)
    {
            printf("some problem with the mister scalar, maybe this core doesn't support it\n");
            exit(0);
    }


    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = ms->width;
    c->height = ms->height;
    /* frames per second */
    c->time_base = (AVRational){1, 20};
    c->framerate = (AVRational){20, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 1;
    //c->pix_fmt = AV_PIX_FMT_RGB24;
    c->pix_fmt = AV_PIX_FMT_YUV420P;


    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);

    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
        exit(1);
    }

    f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

    /* encode 1 second of video */
    for (i = 0; i < 25*seconds; i++) {
        fflush(stdout);

        /* make sure the frame data is writable */
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);

//	mister_scalar_read(ms,frame->data[0]);
	//clock_t begin = clock();

        mister_scalar_read_frame(ms,frame);

        //clock_t end = clock();
        //double time_spent = (double)(end - begin); //in microseconds
        //printf("frame time: %lf\n",time_spent/1000);
//        mister_scalar_read_yuv(ms,frame->linesize[0],frame->data[0],frame->linesize[1], frame->data[1],frame->linesize[2],frame->data[2]);

        /* prepare a dummy image */
        /* Y */
	/*
        for (y = 0; y < c->height; y++) {
            for (x = 0; x < c->width; x++) {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }
        */
        /* Cb and Cr */
	/*
        for (y = 0; y < c->height/2; y++) {
            for (x = 0; x < c->width/2; x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }
	*/

        frame->pts = i;

        /* encode the image */
        encode(c, frame, pkt, f);
    }

    /* flush the encoder */
    encode(c, NULL, pkt, f);

    /* add sequence end code to have a real MPEG file */
    fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    return 0;
}
