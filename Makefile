CC = gcc -m32
AS = as --32
LD = ld -m elf_i386
CFLAGS = -Wall -Wextra -pedantic -nostdlib -nostartfiles -static -fno-pic \
				 -include src/start.h
LDFLAGS =

DEBUG_CFLAGS = -g3
DEBUG_LDFLAGS =

OPT_CFLAGS = -Os -ffunction-sections -fdata-sections \
						 -fno-unwind-tables -fno-asynchronous-unwind-tables \
						 -fomit-frame-pointer -fno-stack-protector \
						 -mpreferred-stack-boundary=2
OPT_LDFLAGS = --gc-sections -s

.PHONY: default all opt debug clean
.PRECIOUS: build/%.o build/opt/%.o build/debug/%.o
default: debug

SOURCES = $(wildcard src/day[0-2][0-9].c)
OPT_SOLVERS = ${SOURCES:src/%.c=bin/opt/%}
DEBUG_SOLVERS = ${SOURCES:src/%.c=bin/debug/%}

all: opt debug
opt: ${OPT_SOLVERS}
debug: ${DEBUG_SOLVERS}

clean:
	rm -rf bin build

build:
	mkdir build

build/opt build/debug: | build
	mkdir $@

build/%.o: src/%.s | build
	${AS} $^ -o $@

build/opt/%.o: src/%.c src/start.h | build/opt
	${CC} ${CFLAGS} ${OPT_CFLAGS} -c $< -o $@

build/debug/%.o: src/%.c src/start.h | build/debug
	${CC} ${CFLAGS} ${DEBUG_CFLAGS} -c $< -o $@

bin:
	mkdir bin

bin/opt bin/debug: | bin
	mkdir $@

# The order of the dependencies here is very important: the linker script must
# come first (to be the sole argument to -T), and elf.o must come next (to be
# the first thing in the output).
bin/opt/%: src/link.ld build/elf.o build/start.o build/opt/%.o | bin/opt
	${LD} ${LDFLAGS} ${OPT_LDFLAGS} -T $^ -o $@

bin/debug/%: build/start.o build/debug/%.o | bin/debug
	${LD} ${LDFLAGS} ${DEBUG_LDFLAGS} $^ -o $@
