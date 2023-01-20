# Makefile for building embedded application.
# by Brian Fraser

OUTFILE = led
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

help:
	@echo "Build Hello World program for BeagleBone"
	@echo "Targets include all, app, and clean."

all: app nestedDir done

app:
	$(CC_C) $(CFLAGS) LED_Controller.c -o $(OUTDIR)/$(OUTFILE)
	ls -l $(OUTDIR)/$(OUTFILE)

nestedDir:
	# Run make in a nested folder
	make --directory=myNestedFolder

done: 
	@echo "Finished building application."

clean:
	rm $(OUTDIR)/$(OUTFILE)