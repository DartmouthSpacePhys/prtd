CC = gcc
CFLAGS = -std=gnu99 -pipe -O2 -Wall
LDFLAGS = -pipe -Wall -Wno-unused-result -L/opt/local/lib
LDLIBS = -lfftw3 -lm

SOURCES = prtd.c dynprtd.c dynprtd_helpers.c prtd_1ch.c prtd_2ch.c
OBJECTS = $(SOURCES:.c=.o)
EXECS = prtd dynprtd prtd_1ch prtd_2ch

all: $(SOURCES) $(EXECS)

prtd: prtd.o
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

dynprtd: dynprtd.o dynprtd_helpers.o
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

prtd_1ch: prtd_1ch.o
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

prtd_2ch: prtd_2ch.o
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

.c.o:
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	rm *.o $(EXECS)
