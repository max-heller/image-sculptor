CC = gcc
CFLAGS = -O2 -g -g3 -Wall -Wextra -Wcast-align -Wconversion
CFLAGS += -Winline -Wfloat-equal -Wnested-externs
CFLAGS += -pedantic -std=gnu99 -D_GNU_SOURCE

OBJS = lodepng.o
EXECS = sculptor
LIBS = -lm

.PHONY: all clean

all: $(EXECS)

sculptor: sculptor.c sculptor.h $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

lodepng.o: lodepng.c lodepng.h

clean:
	rm -f *~ *.o $(EXECS)