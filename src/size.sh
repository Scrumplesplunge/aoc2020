#!/bin/bash

find bin/opt -type f -executable -printf '%f %5s\n' | sort
find bin/opt -type f -executable -printf '%s\n' |
awk '{count += $1} END {print "total " count}'
