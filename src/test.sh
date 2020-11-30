#!/bin/bash

mode=${1?}

make -q "$mode" || make "$mode"

function solve {
  local day="${1?}"
  local puzzle="${2?}"
  printf "$day($puzzle)..."
  local solver="bin/$mode/$day"
  local input="puzzles/$day/$puzzle.input"
  local output="puzzles/$day/$puzzle.output"
  if ! [[ -x "$solver" ]]; then
    printf " \x1b[31m$solver does not exist or is not an executable\x1b[0m\n"
    return 1
  fi
  if ! [[ -r "$input" ]]; then
    printf " \x1b[31m$input does not exist or is not readable\x1b[0m\n"
    return 1
  fi
  if ! [[ -r "$output" ]]; then
    printf " \x1b[31m$output does not exist or is not readable\x1b[0m\n"
    return 1
  fi
  if "$solver" < "$input" | colordiff --color=yes "$output" - > /tmp/diff; then
    printf " \x1b[32mPASSED\x1b[0m\n"
  else
    printf " \x1b[31mFAILED\x1b[0m: (\x1b[31mwant\x1b[0m, \x1b[32mgot\x1b[0m)\n"
    cat /tmp/diff
  fi
}

for day in day{01..25}; do
  if ! [[ -d "puzzles/$day" ]]; then
    continue
  fi
  find "puzzles/$day" -name '*.output' | while read output; do
    puzzle="$(basename -s '.output' "$output")"
    solve "$day" "$puzzle"
  done
done
