# makefile to fail if any command in pipe is failed.
SHELL = /bin/bash -o pipefail

# using gcc version 5.4.1 20161213 (Linaro GCC 5.4-2017.01-rc2)
BASE    = arm-linux-gnueabihf

CC      = $(BASE)-gcc
LD      = $(CC)
STRIP   = $(BASE)-strip

ifeq ($(V),1)
        Q :=
else
        Q := @
endif

INCLUDE = -I./


# use pkg-config for getting CFLAGS and LDLIBS
FFMPEG_LIBSA=    \
                libavformat                        \
                libavcodec                         \
                libswresample                      \
                libswscale                         \
                libavutil                          

FFMPEG_LIBS =   \
                libavformat                         \
                libavcodec                         \
                libavutil                          

CFLAGS += -Wall -g
CFLAGS := $(shell $(BASE)-pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
LDLIBS := $(shell $(BASE)-pkg-config --libs $(FFMPEG_LIBS)) $(LDLIBS)

EXAMPLES=       encode_video

UNUSED=                avio_dir_cmd                       \
                avio_reading                       \
                decode_audio                       \
                decode_video                       \
                demuxing_decoding                  \
                encode_audio                       \
                encode_video                       \
                extract_mvs                        \
                filtering_video                    \
                filtering_audio                    \
                http_multiclient                   \
                hw_decode                          \
                metadata                           \
                muxing                             \
                remuxing                           \
                resampling_audio                   \
                scaling_video                      \
                transcode_aac                      \
                transcoding                        \

SRC = ../mister_scalar.c $(wildcard ../memtool/*.c)

SRC:= $(SRC:.c=.o)
OBJS1=$(addsuffix .o,$(EXAMPLES))
OBJS=$(OBJS1) $(SRC)
# the following examples make explicit use of the math library
avcodec:           LDLIBS += -lm 
encode_audio:      LDLIBS += -lm -static
encode_video:      LDLIBS += -lm  $(SRC)
muxing:            LDLIBS += -lm
resampling_audio:  LDLIBS += -lm

.phony: all clean-test clean

all: $(OBJS) $(EXAMPLES)

clean-test:
	$(RM) test*.pgm test.h264 test.mp2 test.sw test.mpg

clean: clean-test
	$(RM) $(EXAMPLES) $(OBJS)
