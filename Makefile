CC = gcc -m32
AS = as --32
LD = ld -m elf_i386
CFLAGS = -Wall -Wextra -pedantic -nostdlib -nostartfiles -static -fno-pic
LDFLAGS =

DEBUG_CFLAGS = -g3
DEBUG_LDFLAGS =

OPT_CFLAGS = -Os -ffunction-sections -fdata-sections \
						 -fno-unwind-tables -fno-asynchronous-unwind-tables \
						 -fomit-frame-pointer -mpreferred-stack-boundary=2
OPT_LDFLAGS = --gc-sections -s

.PHONY: default all opt debug clean
.PRECIOUS: obj/%.o obj/opt/%.o obj/debug/%.o
default: debug

SOURCES = $(wildcard src/day[0-2][0-9].c)
OPT_SOLVERS = ${SOURCES:src/%.c=bin/opt/%}
DEBUG_SOLVERS = ${SOURCES:src/%.c=bin/debug/%}

all: opt debug
opt: ${OPT_SOLVERS}
debug: ${DEBUG_SOLVERS}

clean:
	rm -rf bin obj

obj:
	mkdir obj

obj/opt obj/debug: | obj
	mkdir $@

obj/%.o: src/%.s | obj
	${AS} $^ -o $@

obj/opt/%.o: src/%.c | obj/opt
	${CC} ${CFLAGS} ${OPT_CFLAGS} -c $^ -o $@

obj/debug/%.o: src/%.c | obj/debug
	${CC} ${CFLAGS} ${DEBUG_CFLAGS} -c $^ -o $@

bin:
	mkdir bin

bin/opt bin/debug: | bin
	mkdir $@

# The order of the dependencies here is very important: the linker script must
# come first (to be the sole argument to -T), and elf.o must come next (to be
# the first thing in the output).
bin/opt/%: src/link.ld obj/elf.o obj/start.o obj/opt/%.o | bin/opt
	${LD} ${LDFLAGS} ${OPT_LDFLAGS} -T $^ -o $@

bin/debug/%: obj/start.o obj/debug/%.o | bin/debug
	${LD} ${LDFLAGS} ${DEBUG_LDFLAGS} $^ -o $@
