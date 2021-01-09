# Advent of Code 2020

These are my solutions to Advent of Code 2020. To challenge myself, I am going
to try to solve every problem in plain C, with no libraries, using only `read`,
`write` and `exit` to interact with the outside world. Since I'm not using
a standard library, some parts of the code must necessarily be non-portable, so
I am assuming x86. The result of this overall approach is very tiny binaries:

    $ make opt
    ...
    $ src/size.sh
    day01   559
    day02   659
    day03   524
    day04  1049
    day05   404
    day06   340
    day07  1058
    day08  1094
    day09   897
    day10   732
    day11  1296
    day12   980
    day13  1316
    day14  1384
    day15   421
    day16  1456
    day17  1450
    day18   920
    day19  1068
    day20  2612
    day21  1494
    day22  1191
    day23   616
    day24   696
    day25   762
    total 24978

## Build

  * `make`, `make debug` - build the debug versions of each solver.
  * `make opt` - build the optimized versions.
  * `make all` - build both debug and optimized versions of each solver.

## Test

  * `src/test.sh [debug|opt]` - Run all solvers with all available inputs.
