# Author(s):
# - James Churchill <pelrun@gmail.com>
# - Dylan Turner <dylantdmt@gmail.com>
# Description: Build the DisPel disassembler

# Settings

## Compiler settings

CC :=		gcc
CFLAGS :=	-Wall -Werror -O2 \
			-Iinclude
LD :=		gcc
LDFLAGS :=	

## Project settings

SRC :=		$(wildcard src/*.c)
HFILES :=	$(wildcard include/*.h)
OBJS :=		$(subst src/,obj/,$(subst .c,.o,$(SRC)))
OBJNAME :=	dispel

# Targets

## Helper Targets

.PHONY: all
all: $(OBJNAME)

.PHONY: clean
clean:
	rm -rf obj/
	rm -rf $(OBJNAME)

obj/%.o: src/%.c $(HFILES)
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

## Main target(s)

$(OBJNAME): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

