OUTFILE = A2
OUTFILE2 = noworky
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -pthread -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow

all: app done

app:
	$(CC_C) $(CFLAGS)  periodTimer.c Global.c Utility.c HistoryArrayAlloc.c LightSensorA2D.c PotDrivers.c I2C.c UDPServer.c main.c -o $(OUTDIR)/$(OUTFILE)
	$(CC_C) $(CFLAGS) noworky.c -o $(OUTDIR)/$(OUTFILE2)
	ls -l $(OUTDIR)/$(OUTFILE)

done: 
	@echo "Finished building application."

clean:
	rm $(OUTDIR)/$(OUTFILE)