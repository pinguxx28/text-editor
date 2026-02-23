#include "cursor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "die.h"
#include "line.h"
#include "debug.h"
#include "buffer.h"

static cursor_t saved_cursor = { 0, 0, 0 };

cursor_t *init_cursor(void) {
	cursor_t *cursor = malloc(sizeof(cursor_t));
	if (cursor == NULL) die("malloc in init_cursor");

	cursor->x = 0;
	cursor->y = 0;
	cursor->rightmost = 0;

	return cursor;
}

void free_cursor(cursor_t *cursor) {
	if (cursor == NULL) die("cursor is null in free_cursor");

	free(cursor);
	cursor = NULL;
}

void save_cursor_pos(cursor_t *cursor) {
	if (cursor == NULL) die("cursor is null in save_cursor_pos");
	saved_cursor = *cursor;
}

void restore_cursor_pos(cursor_t *cursor) {
	if (cursor == NULL) die("cursor is null in restore_cursor_pos");

	cursor->x = saved_cursor.x;
	cursor->y = saved_cursor.y;

	set_cursor_pos(cursor);
}

void set_cursor_pos(cursor_t *cursor) {
	if (cursor == NULL) die("cursor is null in set_cursor_pos");

	char buf[32];

    int len = snprintf(
			buf, sizeof(buf), "\033[%ld;%ldH",
			cursor->y + 1, cursor->x + 1);
    if (len < 0) die("snprintf in set_cursor_pos");

    if (write(STDOUT_FILENO, buf, len) == -1) {
        die("write in set_cursor_pos");
	}
}


static void move_vertical(cursor_t *cursor, buffer_t *buffer, vert_move_t vert_move, int direction) {
	if (cursor == NULL) die("cursor is null in move_vertical");
	if (buffer == NULL) die("buffer is null in move_vertical");
	if (buffer->lines == NULL) die("buffer lines is null in move_vertical");
	if (buffer->lines[cursor->y + direction] == NULL) die("buffer->lines[cy+d] is null in move_vertical");
	if (direction != -1 && direction != 1) die("invalid direction in move_vertical");

	cursor->y += direction;

	const size_t len = get_line_from_buffer(buffer, cursor->y)->length;

	switch (vert_move) {
		case STAY:
			if (cursor->x > len || len < cursor->rightmost) cursor->x = len;
			else cursor->x = cursor->rightmost;
			break;
		case MIN:
			cursor->x = 0;
			cursor->rightmost = cursor->x;
			break;
		case MAX:
			cursor->x = len;
			cursor->rightmost = cursor->x;
			break;
	}

	set_cursor_pos(cursor);
}

void move_up(cursor_t *cursor, buffer_t *buffer, vert_move_t vert_move) {
	if (cursor == NULL) die("cursor is null in move_up");
	if (buffer == NULL) die("buffer is null in move_up");
	if (cursor->y <= 0) return;
	move_vertical(cursor, buffer, vert_move, -1);
}

void move_down(cursor_t *cursor, buffer_t *buffer, vert_move_t vert_move) {
	if (cursor == NULL) die("cursor is null in move_down");
	if (buffer == NULL) die("buffer is null in move_down");
	if (buffer->lines == NULL) die("buffer lines is null in move_down");
	if (buffer->lines[cursor->y] == NULL) die("buffer->lines[cursor->y] is null in move_down");
	if (cursor->y >= MAX_NUM_OF_BUFFER_LINES) return;
	if (cursor->y > buffer->size - 1) return;

	if (cursor->y == buffer->size - 1) {
		cursor->x = buffer->lines[cursor->y]->length;
		set_cursor_pos(cursor);
	} else {
		move_vertical(cursor, buffer, vert_move, 1);
	}
}

void move_right(cursor_t *cursor, buffer_t *buffer) {
	if (cursor == NULL) die("cursor is null in move_right");
	if (buffer == NULL) die("buffer is null in move_right");
	if (cursor->x >= MAX_LINE_CAPACITY) return;
	if (cursor->x >= get_line_from_buffer(buffer, cursor->y)->length) return;

	if (write(STDOUT_FILENO, "\033[1C", 4) == -1) {
		die("write in move_right");
	}

	cursor->x++;
	cursor->rightmost = cursor->x;
}

void move_left(cursor_t *cursor, buffer_t *buffer) {
	if (cursor == NULL) die("cursor is null in move_left");
	if (buffer == NULL) die("buffer is null in move_left");
	if (cursor->x <= 0) return;
	(void) buffer; // ignore warning

	if (write(STDOUT_FILENO, "\033[1D", 4) == -1) {
		die("write in move_left");
	}

	cursor->x--;
	cursor->rightmost = cursor->x;
}

