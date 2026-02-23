#include "die.h"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "rawmode.h"

void die(const char *str) {
	DEBUG(str);
	DEBUG("\n");
	terminal_disable_raw_mode();
	exit(EXIT_FAILURE);
}
