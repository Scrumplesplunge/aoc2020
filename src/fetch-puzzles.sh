#!/bin/bash

users=(
  agata-borkowska-clark
  JoBoCl
  Scrumplesplunge
)

function fetch {
  local user="${1?}"
  if [[ -d "/tmp/$user-aoc2020" ]]; then
    # Try `git pull` instead, since it may be faster.
    (cd "/tmp/$user-aoc2020"; git pull) && return 0;
    # If that fails, start from scratch.
    rm -rf "/tmp/$user-aoc2020"
  fi
  git clone "git@github.com:$user/aoc2020.git" "/tmp/$user-aoc2020"
}

function copy {
  local user="${1?}"
  rsync -rv "/tmp/$user-aoc2020/puzzles/" "puzzles/"
}

for user in "${users[@]}"; do
  fetch "$user" && copy "$user"
done
