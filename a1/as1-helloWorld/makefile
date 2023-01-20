OUTFILE = hello
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -pthread -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow

all: app done

app:
	$(CC_C) $(CFLAGS) JoyStick_Game.c -o $(OUTDIR)/$(OUTFILE)
	ls -l $(OUTDIR)/$(OUTFILE)

done: 
	@echo "Finished building application."

clean:
	rm $(OUTDIR)/$(OUTFILE)