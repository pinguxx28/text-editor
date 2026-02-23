#include "rawmode.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

static struct termios original;

void terminal_disable_raw_mode(void) {
	// just make sure to reset
	original.c_iflag |= IXON;
	original.c_lflag |= ISIG | ICANON | ECHO;

	if (tcsetattr(STDIN_FILENO, TCSANOW, &original) == -1) {
		perror("tcsetattr inside of terminal_disable_raw_mode");
	}
}

void terminal_enable_raw_mode(void) {
	// OBSERVE we can't use die()
	// because die depends on this file so it would lead to a circular include
	if (tcgetattr(STDIN_FILENO, &original) == -1) {
		perror("tcgetattr in terminal_enable_raw_mode");
		exit(EXIT_FAILURE);
	}

	// SETTING UP ATEXIT FUNCTION
	if (atexit(terminal_disable_raw_mode) != 0) {
		perror("atexit in terminal_enable_raw_mode");
		exit(EXIT_FAILURE);
	}

	struct termios raw_mode = original; // copy original
	raw_mode.c_iflag &= ~(IXON);
	raw_mode.c_lflag &= ~(ISIG | ICANON | ECHO);
	raw_mode.c_cc[VMIN] = 0;
	raw_mode.c_cc[VTIME] = 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_mode) == -1) {
		terminal_disable_raw_mode();
		perror("tcsetattr in terminal_enable_raw_mode");
		exit(EXIT_FAILURE);
	}
}

