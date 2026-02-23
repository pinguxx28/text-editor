#!/bin/bash

# Write stderr to debug.log
# Open debug.log if ./main exited abnormally

# Run program, capture stderr in debug.log
./main test.txt 2> debug.log
exit_code=$?

# Count lines in debug.log
errors=$(wc -l < debug.log)

if [ "$exit_code" -ne 0 ]; then
    # Non-zero exit → failure
    if [ "$errors" -gt 0 ]; then
        # Red bold message
        printf "\033[1;31mEncountered %d error(s)\033[0m\n" "$errors"
        cat debug.log
    else
        printf "\033[1;31mProgram failed but no debug messages\033[0m\n"
    fi
else
    # Zero exit → success
    if [ "$errors" -gt 0 ]; then
        printf "\033[1;34mDebug prints:\033[0m\n"   # Blue message for debug
        cat debug.log
    fi
fi
