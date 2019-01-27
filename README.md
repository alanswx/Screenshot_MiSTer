# Screenshot_MiSTer
This is a utility to screen shot MiSTer from linux. Newer version of the ASCAL open source scalar stores buffers in memory starting at 512MB. Linux on the mister is setup to only use the first 512MB. We can read the second half using /dev/mem
at 512MB there is an ASCAL buffer if the ASCAL is running on the FPGA. It has information about the image format. After the header are RGB bytes.

This code is adapted from https://github.com/pengutronix/memtool/

Ascal header has a frame counter at 0x20000005.
* b0 : Interlaced
* b1 : Field number
* b2 : Horizontal downscaled
* b3 : Vertical downscaled
* b4 : Triple buffered
* b7-5 : Frame counter

Right now the code ignores the second and third buffers, but here is an explanation of how it could work, esp for the video version:

With triple buffering, there are also frames at 2080_0000 and 2100_0000

The header is updated before writing the frame (a few tens to hundred of µs before)
I don't know how fast is the ARM for copying memory. If it fast enough, for a stable image:
- poll the header at 0x20000005 until the frame counter has changed.
- with triple buffering, that means that the two other buffers won't change while the first buffer is being updated
- with single buffer, let's hope that the CPU can copy data faster than the input pixel rate.
- The counter should not have changed after the end of the copy.

Once this gets nice and automated, we can slide it into MiSTer so that we can use the Print Screen button, or something to screenshot.

Thanks to Grabulosaure for all the help!

