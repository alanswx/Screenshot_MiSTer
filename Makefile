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

INCLUDE	= -I./

PRJ = screensht
SRC = $(wildcard *.c) $(wildcard memtool/*.c)
SRC2 = $(wildcard *.cpp)

VPATH	= ./:./support/minimig:./support/sharpmz:./support/archie:./support/st:./support/x86:./support/snes

OBJ	= $(SRC:.c=.c.o) $(SRC2:.cpp=.cpp.o) 
DFLAGS	= $(INCLUDE) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -DVDATE=\"`date +"%y%m%d"`\"
CFLAGS	= $(DFLAGS) -Wall -Wextra -Wno-strict-aliasing -c -O3

IMLIB2_LIB  = -Llib/imlib2 -lfreetype -lbz2 -lpng16 -lz -lImlib2

LFLAGS	= -lc -lstdc++ -lrt $(IMLIB2_LIB)



$(PRJ): $(OBJ)
	$(Q)$(info $@)
	$(Q)$(LD) -o $@ $+ $(LFLAGS)
	$(Q)cp $@ $@.elf
	$(Q)$(STRIP) $@

clean:
	$(Q)rm -f *.elf *.map *.lst *.user *~ $(PRJ)
	$(Q)rm -rf obj .vs DTAR* x64
	$(Q)find . \( -name '*.o' -o -name '*.d' -o -name '*.bak' -o -name '*.rej' -o -name '*.org' \) -exec rm -f {} \;

cleanall:
	$(Q)rm -rf $(OBJ) $(DEP) *.elf *.map *.lst *.bak *.rej *.org *.user *~ $(PRJ)
	$(Q)rm -rf obj .vs DTAR* x64
	$(Q)find . -name '*.o' -delete
	$(Q)find . -name '*.d' -delete

%.c.o: %.c
	$(Q)$(info $<)
	$(Q)$(CC) $(CFLAGS) -std=gnu99 -o $@ -c $< 2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):\([0-9]\+\):/\1(\2,\ \3):/g'

%.cpp.o: %.cpp
	$(Q)$(info $<)
	$(Q)$(CC) $(CFLAGS) -std=gnu++14 -o $@ -c $< 2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):\([0-9]\+\):/\1(\2,\ \3):/g'

-include $(DEP)
%.c.d: %.c
	$(Q)$(CC) $(DFLAGS) -MM $< -MT $@ -MT $*.c.o -MF $@ 2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):\([0-9]\+\):/\1(\2,\ \3):/g'

%.cpp.d: %.cpp
	$(Q)$(CC) $(DFLAGS) -MM $< -MT $@ -MT $*.cpp.o -MF $@ 2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):\([0-9]\+\):/\1(\2,\ \3):/g'

# Ensure correct time stamp
main.cpp.o: $(filter-out main.cpp.o, $(OBJ))
