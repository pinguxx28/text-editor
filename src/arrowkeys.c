#include "arrowkeys.h"
#include <unistd.h>

arrow_key_t get_arrow_key(char c1, char c2) {
	// arrow keys are interpreted as ESC,91,[65-68]
	
	if (c1 != 91) return NO;

	if (c2 == 65) return UP;
	else if (c2 == 66) return DOWN;
	else if (c2 == 67) return RIGHT;
	else if (c2 == 68) return LEFT;
	else return NO;
}

void move_with_arrows(arrow_key_t arrow_key, cursor_t *cursor, buffer_t *buffer) {
	switch (arrow_key) {
		case NO: break;
		default: break;
		case UP:    move_up(cursor, buffer, STAY);    break;
		case DOWN:  move_down(cursor, buffer, STAY);  break;
		case RIGHT: move_right(cursor, buffer); break;
		case LEFT:  move_left(cursor, buffer);  break;
	}
}
