#!/usr/bin/bash

set -e
# -Werror
gcc  -Wall -Wextra -Wpedantic -lm -lcurses main.c 
./a.out
