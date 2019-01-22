# Screenshot_MiSTer
This is a utility to screen shot MiSTer from linux, hard coded right now. The ASCAL open source scalar stores buffers in memory starting at 512MB. Linux is setup to only use the first 512MB.

This code is adapted from https://github.com/pengutronix/memtool/

it opens /dev/mem and reads bytes from the framebuffer. Right now it has to be hardcoded based on the video output. The format isn't quite right yet.

The scalar is supposed to store a header that includes the size of the image in the first few bytes of memory. There seems to be a bug where it isn't showing up. Once that is fixed we can make this much cleaner.

Once this gets nice and automated, we can slide it into MiSTer so that we can use the Print Screen button, or something to screenshot.

Thanks to Grabulosaure for all the help!

