#pragma once
#include "cursor.h"
#include "buffer.h"

typedef enum {
	NO,
	UP,
	DOWN,
	RIGHT,
	LEFT
} arrow_key_t;

arrow_key_t get_arrow_key(char c1, char c2);
void move_with_arrows(arrow_key_t arrow_key, cursor_t *cursor, buffer_t *buffer);
