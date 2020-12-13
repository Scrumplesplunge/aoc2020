#!/bin/bash

users=(
  agata-borkowska-clark:aoc2020:.
  JoBoCl:aoc2020:.
  Scrumplesplunge:aoc2020:.
  RNPValinor:adventcode:_2020
)

function fetch {
  local user="${1?}"
  local repository="${2?}"
  if [[ -d "/tmp/$user-aoc2020" ]]; then
    # Try `git pull` instead, since it may be faster.
    (cd "/tmp/$user-aoc2020"; git pull) && return 0;
    # If that fails, start from scratch.
    rm -rf "/tmp/$user-aoc2020"
  fi
  git clone "git@github.com:$user/$repository.git" "/tmp/$user-aoc2020"
}

function copy {
  local user="${1?}"
  local directory="${2?}"
  rsync --filter='- *, + *.input, + *.output' -rv "/tmp/$user-aoc2020/$directory/puzzles/" "puzzles/"
}

for info in "${users[@]}"; do
  IFS=: read user repository directory <<< "$info"
  echo "git@github.com:$user/$repository/$directory/puzzles"
  fetch "$user" "$repository" && copy "$user" "$directory"
done

# Clean up the solutions.
rename puzzle rob puzzles/*/*
rename dammit-joe rob puzzles/*/*
rm puzzles/*/*.{test,exponential}
# Remove trailing whitespace and ensure that the file ends with a newline.
sed -i 's/\s*$//;a\' puzzles/*/*
