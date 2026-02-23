#pragma once
#include <stddef.h>
#include "buffer.h"

typedef struct {
	size_t x; // column
	size_t y; // row
	size_t rightmost;
} cursor_t;

typedef enum {
	STAY, // stay on the same
	MIN,  // go to the start of the line
	MAX   // go to the end of the line
} vert_move_t;

cursor_t *init_cursor(void);
void free_cursor(cursor_t *cursor);

void save_cursor_pos(cursor_t *cursor);
void restore_cursor_pos(cursor_t *cursor);

void set_cursor_pos(cursor_t *cursor);

void move_up(cursor_t *cursor, buffer_t *buffer, vert_move_t vert_move);
void move_down(cursor_t *cursor, buffer_t *buffer, vert_move_t vert_move);
void move_right(cursor_t *cursor, buffer_t *buffer);
void move_left(cursor_t *cursor, buffer_t *buffer);
