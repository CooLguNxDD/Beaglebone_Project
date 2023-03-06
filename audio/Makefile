# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
TARGET= wave_player

SOURCES= Utility.c periodTimer.c audioMixer_template.c BeatController.c Accelerometer_Controller.c JoyStickController.c UDPSer.c main.c


PUBDIR = $(HOME)/cmpt433/public/myApps
LIBDIR = $(HOME)/cmpt433/public
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow


# Asound process:
# get alibsound2 lib on target:
# 	# apt-get install libasound2
# Copy target's /usr/lib/arm-linux-gnueabihf/libasound.so.2.0.0
#      to host  ~/public/asound_lib_BBB/libasound.so
# Copy to just base library:

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB


# -pg for supporting gprof profiling.
#CFLAGS += -pg

all: wav node
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET)  $(LFLAGS) -lpthread -lasound

fftw3:
	apt-get install fftw3
	mkdir -p $(LIBDIR)/fftw3/
	cp /usr/lib/arm-linux-gnueabihf/libfftw3.so.3.5.8 $(LIBDIR)/fftw3/

# Copy wave files to the shared folder
wav:
	mkdir -p $(PUBDIR)/beatbox-wav-files/
	cp wave-files/* $(PUBDIR)/beatbox-wav-files/
node:
	mkdir -p $(PUBDIR)/beatbox-server-copy/
	cp -R as3-server/* $(PUBDIR)/beatbox-server-copy/
	cd $(PUBDIR)/beatbox-server-copy/ && sudo npm install

clean:
	rm -f $(OUTDIR)/$(TARGET)