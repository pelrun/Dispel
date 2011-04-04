CC=gcc
CFLAGS=-Wall -O2
LDFLAGS=
SOURCES=main.c 65816.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=dispel.exe

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o ${EXECUTABLE}
