# Advent of Code 2020

These are my solutions to Advent of Code 2020. To challenge myself, I am going
to try to solve every problem in plain C, with no libraries, using only `read`,
`write` and `exit` to interact with the outside world. The result of this is
very tiny binaries:

![Most binaries are under 2KiB, some are under 500B, and the total size of all
binaries is under 28KiB](sizes.png)

## Build

  * `make`, `make debug` - build the debug versions of each solver.
  * `make opt` - build the optimized versions.
  * `make all` - build both debug and optimized versions of each solver.

## Test

  * `src/test.sh [debug|opt]` - Run all solvers with all available inputs.
