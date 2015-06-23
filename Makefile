CC=gcc
CFLAGS=-Wall -O2
LDFLAGS=
SOURCES=main.c 65816.c
OBJECTS=$(SOURCES:.c=.o)
CAN_INSTALL = no
ifeq ($(OS),Windows_NT)
	EXECUTABLE = dispel.exe
else
	EXECUTABLE = dispel
	UNAME_S = $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CAN_INSTALL = yes
	endif
endif
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
install:
	cp -v $(EXECUTABLE) /usr/local/bin
	#FIXME It ALWAYS copies dispel to /usr/bin, regardless of OS
uninstall:
	rm -rf /usr/local/bin/$(EXECUTABLE)
clean:
	rm *.o ${EXECUTABLE}
