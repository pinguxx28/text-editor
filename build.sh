#!/bin/bash

CC="gcc"
FILES="src/*.c"
OUTPUT="-o main"
FLAGS="-Wall -Wextra -Wpedantic -Werror"

if [[ "$1" == "debug" ]]; then
	DEBUG="-g -fsanitize=address"
else
	DEBUG=""
fi

set -xe
$CC $FILES $OUTPUT $FLAGS $DEBUG
