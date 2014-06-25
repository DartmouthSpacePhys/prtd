prtd Notes
=================================
Spencer Hatch
Jun 25 2014

What this is
--------------
This program processes (i.e., performs a Fast Fourier Transform on) real-time data via a lock file that is, by default, located in /tmp/rtd. I recommend you leave it that way, but you can change these defaults by editing hf2_config.input in the top directory of this repository. Further, you'll want to make sure that the "monitor file" created by either acq_c or acq_c2.0 (the program responsible for actually acquiring data--the monitor file has historically been associated with option -m in the acquisition programs) has the same name and directory as the file for which prtd looks for processing data. As noted, you may *specify* which file to use as a memory-mapped monitor file with acq_c*; prtd, however, has the name of the file *hard-wired* into the code, so you'll need to get into the source if you want to change the monitor file from which prtd reads data.


hf2_config.intput
--------------------
There are two directories to specify in this file:

###TMP_DIR
This is the directory where prtd will create the file 'process_real_time_data_running' so that other instances of prtd will know that there is already a copy running. It is set to /tmp/rtd in the included hf2_config.input file.

###DATA_DIR
This is the directory where prtd will create a file with FFT information to be used by rtdgui.


Dependencies
--------------
###prtd
You'll need
*FFTW (FFTW3 known to work)

###rtdgui and acq_c
*FLTK libraries

*GL libraries
It wasn't always the case that you needed both GL and FLTK libraries, since they used to be one and the same. Somewhere along the way, as of the date this was written (Jun 25 2014), you need both

*FLU library
This one is harder to come by, though a working version of FLU should be available in the rtdgui repository.

Syntax
-----------

sudo /path/to/prtd /path/to/hf2_config.input

