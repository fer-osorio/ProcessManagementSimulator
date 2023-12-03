all: ProcssMngmntSim


WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2
SOURCE = main.c PrintArray.c
HEADERS = PrintArray.h

ProcssMngmntSim: Makefile $(SOURCE) $(HEADERS)
	$(CC) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) $(SOURCE)

clean:
	rm -f ProcssMngmntSim

# Builder will call this to install the application before running.
install:
	echo "Installing is not supported"

# Builder uses this target to run your application.
run:
	./ProcssMngmntSim

