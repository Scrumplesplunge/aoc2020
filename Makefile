CC = gcc
AS = as
LD = ld
CFLAGS = -Wall -Wextra -pedantic -nostdlib -nostartfiles -static -fno-pic \
				 -include src/start.h
LDFLAGS =

DEBUG_CFLAGS = -g3
DEBUG_LDFLAGS =

OPT_CFLAGS = -Os -ffunction-sections -fdata-sections \
						 -fno-unwind-tables -fno-asynchronous-unwind-tables \
						 -fomit-frame-pointer -fno-stack-protector \
						 -mpreferred-stack-boundary=4
OPT_LDFLAGS = --gc-sections -s

.PHONY: default all opt debug clean debug_tests opt_tests
.PRECIOUS: build/%.o build/opt/%.o build/debug/%.o
default: debug

SOURCES = $(wildcard src/day[0-2][0-9].c)
OPT_SOLVERS = ${SOURCES:src/%.c=bin/opt/%}
DEBUG_SOLVERS = ${SOURCES:src/%.c=bin/debug/%}

PUZZLES=$(shell find puzzles -name '*.output')
OUTPUTS=$(subst /,.,${PUZZLES:puzzles/%=%})

OPT_OUTPUTS=${OUTPUTS:%=bin/opt/%}
DEBUG_OUTPUTS=${OUTPUTS:%=bin/debug/%}
.PRECIOUS: ${OPT_OUTPUTS} ${DEBUG_OUTPUTS}

all: opt debug
opt: ${OPT_SOLVERS} bin/opt/tests
	cat bin/opt/tests
debug: ${DEBUG_SOLVERS} bin/debug/tests
	cat bin/debug/tests

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
# come first (to be the sole argument to -T).
bin/opt/%: src/link.ld build/opt/%.o | bin/opt
	${LD} ${LDFLAGS} ${OPT_LDFLAGS} -T $^ -o $@
	# GNU strip doesn't have the --strip-sections flag. This strips all of the
	# sections from the output file, making the binaries smaller.
	llvm-strip --strip-sections $@

bin/debug/%: build/debug/%.o | bin/debug
	${LD} ${LDFLAGS} ${DEBUG_LDFLAGS} $^ -o $@

bin/opt/day01.%.output: bin/opt/day01 puzzles/day01/%.input
	bin/opt/day01 <puzzles/day01/$*.input >$@
bin/opt/day01.%.verdict: puzzles/day01/%.output bin/opt/day01.%.output
	src/verdict.sh $^ > $@
bin/opt/day02.%.output: bin/opt/day02 puzzles/day02/%.input
	bin/opt/day02 <puzzles/day02/$*.input >$@
bin/opt/day02.%.verdict: puzzles/day02/%.output bin/opt/day02.%.output
	src/verdict.sh $^ > $@
bin/opt/day03.%.output: bin/opt/day03 puzzles/day03/%.input
	bin/opt/day03 <puzzles/day03/$*.input >$@
bin/opt/day03.%.verdict: puzzles/day03/%.output bin/opt/day03.%.output
	src/verdict.sh $^ > $@
bin/opt/day04.%.output: bin/opt/day04 puzzles/day04/%.input
	bin/opt/day04 <puzzles/day04/$*.input >$@
bin/opt/day04.%.verdict: puzzles/day04/%.output bin/opt/day04.%.output
	src/verdict.sh $^ > $@
bin/opt/day05.%.output: bin/opt/day05 puzzles/day05/%.input
	bin/opt/day05 <puzzles/day05/$*.input >$@
bin/opt/day05.%.verdict: puzzles/day05/%.output bin/opt/day05.%.output
	src/verdict.sh $^ > $@
bin/opt/day06.%.output: bin/opt/day06 puzzles/day06/%.input
	bin/opt/day06 <puzzles/day06/$*.input >$@
bin/opt/day06.%.verdict: puzzles/day06/%.output bin/opt/day06.%.output
	src/verdict.sh $^ > $@
bin/opt/day07.%.output: bin/opt/day07 puzzles/day07/%.input
	bin/opt/day07 <puzzles/day07/$*.input >$@
bin/opt/day07.%.verdict: puzzles/day07/%.output bin/opt/day07.%.output
	src/verdict.sh $^ > $@
bin/opt/day08.%.output: bin/opt/day08 puzzles/day08/%.input
	bin/opt/day08 <puzzles/day08/$*.input >$@
bin/opt/day08.%.verdict: puzzles/day08/%.output bin/opt/day08.%.output
	src/verdict.sh $^ > $@
bin/opt/day09.%.output: bin/opt/day09 puzzles/day09/%.input
	bin/opt/day09 <puzzles/day09/$*.input >$@
bin/opt/day09.%.verdict: puzzles/day09/%.output bin/opt/day09.%.output
	src/verdict.sh $^ > $@
bin/opt/day10.%.output: bin/opt/day10 puzzles/day10/%.input
	bin/opt/day10 <puzzles/day10/$*.input >$@
bin/opt/day10.%.verdict: puzzles/day10/%.output bin/opt/day10.%.output
	src/verdict.sh $^ > $@
bin/opt/day11.%.output: bin/opt/day11 puzzles/day11/%.input
	bin/opt/day11 <puzzles/day11/$*.input >$@
bin/opt/day11.%.verdict: puzzles/day11/%.output bin/opt/day11.%.output
	src/verdict.sh $^ > $@
bin/opt/day12.%.output: bin/opt/day12 puzzles/day12/%.input
	bin/opt/day12 <puzzles/day12/$*.input >$@
bin/opt/day12.%.verdict: puzzles/day12/%.output bin/opt/day12.%.output
	src/verdict.sh $^ > $@
bin/opt/day13.%.output: bin/opt/day13 puzzles/day13/%.input
	bin/opt/day13 <puzzles/day13/$*.input >$@
bin/opt/day13.%.verdict: puzzles/day13/%.output bin/opt/day13.%.output
	src/verdict.sh $^ > $@
bin/opt/day14.%.output: bin/opt/day14 puzzles/day14/%.input
	bin/opt/day14 <puzzles/day14/$*.input >$@
bin/opt/day14.%.verdict: puzzles/day14/%.output bin/opt/day14.%.output
	src/verdict.sh $^ > $@
bin/opt/day15.%.output: bin/opt/day15 puzzles/day15/%.input
	bin/opt/day15 <puzzles/day15/$*.input >$@
bin/opt/day15.%.verdict: puzzles/day15/%.output bin/opt/day15.%.output
	src/verdict.sh $^ > $@
bin/opt/day16.%.output: bin/opt/day16 puzzles/day16/%.input
	bin/opt/day16 <puzzles/day16/$*.input >$@
bin/opt/day16.%.verdict: puzzles/day16/%.output bin/opt/day16.%.output
	src/verdict.sh $^ > $@
bin/opt/day17.%.output: bin/opt/day17 puzzles/day17/%.input
	bin/opt/day17 <puzzles/day17/$*.input >$@
bin/opt/day17.%.verdict: puzzles/day17/%.output bin/opt/day17.%.output
	src/verdict.sh $^ > $@
bin/opt/day18.%.output: bin/opt/day18 puzzles/day18/%.input
	bin/opt/day18 <puzzles/day18/$*.input >$@
bin/opt/day18.%.verdict: puzzles/day18/%.output bin/opt/day18.%.output
	src/verdict.sh $^ > $@
bin/opt/day19.%.output: bin/opt/day19 puzzles/day19/%.input
	bin/opt/day19 <puzzles/day19/$*.input >$@
bin/opt/day19.%.verdict: puzzles/day19/%.output bin/opt/day19.%.output
	src/verdict.sh $^ > $@
bin/opt/day20.%.output: bin/opt/day20 puzzles/day20/%.input
	bin/opt/day20 <puzzles/day20/$*.input >$@
bin/opt/day20.%.verdict: puzzles/day20/%.output bin/opt/day20.%.output
	src/verdict.sh $^ > $@
bin/opt/day21.%.output: bin/opt/day21 puzzles/day21/%.input
	bin/opt/day21 <puzzles/day21/$*.input >$@
bin/opt/day21.%.verdict: puzzles/day21/%.output bin/opt/day21.%.output
	src/verdict.sh $^ > $@
bin/opt/day22.%.output: bin/opt/day22 puzzles/day22/%.input
	bin/opt/day22 <puzzles/day22/$*.input >$@
bin/opt/day22.%.verdict: puzzles/day22/%.output bin/opt/day22.%.output
	src/verdict.sh $^ > $@
bin/opt/day23.%.output: bin/opt/day23 puzzles/day23/%.input
	bin/opt/day23 <puzzles/day23/$*.input >$@
bin/opt/day23.%.verdict: puzzles/day23/%.output bin/opt/day23.%.output
	src/verdict.sh $^ > $@
bin/opt/day24.%.output: bin/opt/day24 puzzles/day24/%.input
	bin/opt/day24 <puzzles/day24/$*.input >$@
bin/opt/day24.%.verdict: puzzles/day24/%.output bin/opt/day24.%.output
	src/verdict.sh $^ > $@
bin/opt/day25.%.output: bin/opt/day25 puzzles/day25/%.input
	bin/opt/day25 <puzzles/day25/$*.input >$@
bin/opt/day25.%.verdict: puzzles/day25/%.output bin/opt/day25.%.output
	src/verdict.sh $^ > $@
bin/debug/day01.%.output: bin/debug/day01 puzzles/day01/%.input
	bin/debug/day01 <puzzles/day01/$*.input >$@
bin/debug/day01.%.verdict: puzzles/day01/%.output bin/debug/day01.%.output
	src/verdict.sh $^ > $@
bin/debug/day02.%.output: bin/debug/day02 puzzles/day02/%.input
	bin/debug/day02 <puzzles/day02/$*.input >$@
bin/debug/day02.%.verdict: puzzles/day02/%.output bin/debug/day02.%.output
	src/verdict.sh $^ > $@
bin/debug/day03.%.output: bin/debug/day03 puzzles/day03/%.input
	bin/debug/day03 <puzzles/day03/$*.input >$@
bin/debug/day03.%.verdict: puzzles/day03/%.output bin/debug/day03.%.output
	src/verdict.sh $^ > $@
bin/debug/day04.%.output: bin/debug/day04 puzzles/day04/%.input
	bin/debug/day04 <puzzles/day04/$*.input >$@
bin/debug/day04.%.verdict: puzzles/day04/%.output bin/debug/day04.%.output
	src/verdict.sh $^ > $@
bin/debug/day05.%.output: bin/debug/day05 puzzles/day05/%.input
	bin/debug/day05 <puzzles/day05/$*.input >$@
bin/debug/day05.%.verdict: puzzles/day05/%.output bin/debug/day05.%.output
	src/verdict.sh $^ > $@
bin/debug/day06.%.output: bin/debug/day06 puzzles/day06/%.input
	bin/debug/day06 <puzzles/day06/$*.input >$@
bin/debug/day06.%.verdict: puzzles/day06/%.output bin/debug/day06.%.output
	src/verdict.sh $^ > $@
bin/debug/day07.%.output: bin/debug/day07 puzzles/day07/%.input
	bin/debug/day07 <puzzles/day07/$*.input >$@
bin/debug/day07.%.verdict: puzzles/day07/%.output bin/debug/day07.%.output
	src/verdict.sh $^ > $@
bin/debug/day08.%.output: bin/debug/day08 puzzles/day08/%.input
	bin/debug/day08 <puzzles/day08/$*.input >$@
bin/debug/day08.%.verdict: puzzles/day08/%.output bin/debug/day08.%.output
	src/verdict.sh $^ > $@
bin/debug/day09.%.output: bin/debug/day09 puzzles/day09/%.input
	bin/debug/day09 <puzzles/day09/$*.input >$@
bin/debug/day09.%.verdict: puzzles/day09/%.output bin/debug/day09.%.output
	src/verdict.sh $^ > $@
bin/debug/day10.%.output: bin/debug/day10 puzzles/day10/%.input
	bin/debug/day10 <puzzles/day10/$*.input >$@
bin/debug/day10.%.verdict: puzzles/day10/%.output bin/debug/day10.%.output
	src/verdict.sh $^ > $@
bin/debug/day11.%.output: bin/debug/day11 puzzles/day11/%.input
	bin/debug/day11 <puzzles/day11/$*.input >$@
bin/debug/day11.%.verdict: puzzles/day11/%.output bin/debug/day11.%.output
	src/verdict.sh $^ > $@
bin/debug/day12.%.output: bin/debug/day12 puzzles/day12/%.input
	bin/debug/day12 <puzzles/day12/$*.input >$@
bin/debug/day12.%.verdict: puzzles/day12/%.output bin/debug/day12.%.output
	src/verdict.sh $^ > $@
bin/debug/day13.%.output: bin/debug/day13 puzzles/day13/%.input
	bin/debug/day13 <puzzles/day13/$*.input >$@
bin/debug/day13.%.verdict: puzzles/day13/%.output bin/debug/day13.%.output
	src/verdict.sh $^ > $@
bin/debug/day14.%.output: bin/debug/day14 puzzles/day14/%.input
	bin/debug/day14 <puzzles/day14/$*.input >$@
bin/debug/day14.%.verdict: puzzles/day14/%.output bin/debug/day14.%.output
	src/verdict.sh $^ > $@
bin/debug/day15.%.output: bin/debug/day15 puzzles/day15/%.input
	bin/debug/day15 <puzzles/day15/$*.input >$@
bin/debug/day15.%.verdict: puzzles/day15/%.output bin/debug/day15.%.output
	src/verdict.sh $^ > $@
bin/debug/day16.%.output: bin/debug/day16 puzzles/day16/%.input
	bin/debug/day16 <puzzles/day16/$*.input >$@
bin/debug/day16.%.verdict: puzzles/day16/%.output bin/debug/day16.%.output
	src/verdict.sh $^ > $@
bin/debug/day17.%.output: bin/debug/day17 puzzles/day17/%.input
	bin/debug/day17 <puzzles/day17/$*.input >$@
bin/debug/day17.%.verdict: puzzles/day17/%.output bin/debug/day17.%.output
	src/verdict.sh $^ > $@
bin/debug/day18.%.output: bin/debug/day18 puzzles/day18/%.input
	bin/debug/day18 <puzzles/day18/$*.input >$@
bin/debug/day18.%.verdict: puzzles/day18/%.output bin/debug/day18.%.output
	src/verdict.sh $^ > $@
bin/debug/day19.%.output: bin/debug/day19 puzzles/day19/%.input
	bin/debug/day19 <puzzles/day19/$*.input >$@
bin/debug/day19.%.verdict: puzzles/day19/%.output bin/debug/day19.%.output
	src/verdict.sh $^ > $@
bin/debug/day20.%.output: bin/debug/day20 puzzles/day20/%.input
	bin/debug/day20 <puzzles/day20/$*.input >$@
bin/debug/day20.%.verdict: puzzles/day20/%.output bin/debug/day20.%.output
	src/verdict.sh $^ > $@
bin/debug/day21.%.output: bin/debug/day21 puzzles/day21/%.input
	bin/debug/day21 <puzzles/day21/$*.input >$@
bin/debug/day21.%.verdict: puzzles/day21/%.output bin/debug/day21.%.output
	src/verdict.sh $^ > $@
bin/debug/day22.%.output: bin/debug/day22 puzzles/day22/%.input
	bin/debug/day22 <puzzles/day22/$*.input >$@
bin/debug/day22.%.verdict: puzzles/day22/%.output bin/debug/day22.%.output
	src/verdict.sh $^ > $@
bin/debug/day23.%.output: bin/debug/day23 puzzles/day23/%.input
	bin/debug/day23 <puzzles/day23/$*.input >$@
bin/debug/day23.%.verdict: puzzles/day23/%.output bin/debug/day23.%.output
	src/verdict.sh $^ > $@
bin/debug/day24.%.output: bin/debug/day24 puzzles/day24/%.input
	bin/debug/day24 <puzzles/day24/$*.input >$@
bin/debug/day24.%.verdict: puzzles/day24/%.output bin/debug/day24.%.output
	src/verdict.sh $^ > $@
bin/debug/day25.%.output: bin/debug/day25 puzzles/day25/%.input
	bin/debug/day25 <puzzles/day25/$*.input >$@
bin/debug/day25.%.verdict: puzzles/day25/%.output bin/debug/day25.%.output
	src/verdict.sh $^ > $@

bin/opt/tests: ${OPT_OUTPUTS:%.output=%.verdict}
	cat $(sort $^) > $@

bin/debug/tests: ${DEBUG_OUTPUTS:%.output=%.verdict}
	cat $(sort $^) > $@
