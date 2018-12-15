CC = gcc
CFLAGS = -O2 -g -g3 -Wall -Wextra -Wcast-align -Wconversion
CFLAGS += -Winline -Wfloat-equal -Wnested-externs
CFLAGS += -pedantic -std=gnu99 -D_GNU_SOURCE

OBJS = lodepng.o
EXECS = sculptor sculptor-perf
LIBS = -lm

.PHONY: all clean

all: $(EXECS)

sculptor: sculptor.c sculptor.h $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

sculptor-perf: sculptor.c sculptor.h $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ -DPERF

lodepng.o: lodepng.c lodepng.h

clean:
	rm -f *~ *.o $(EXECS)