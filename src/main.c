#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "die.h"
#include "keys.h"
#include "buffer.h"
#include "cursor.h"
#include "debug.h"
#include "line.h"
#include "rawmode.h"
#include "arrowkeys.h"

void home_screen(void);
void clear_screen(void);

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: ./main [FILE]\n");
		return EXIT_FAILURE;
	}

	// stderr is unbuffered, so is written to immidiately
	if (setvbuf(stderr, NULL, _IONBF, 0)) die("setvbuf in main");

	buffer_t *buffer = init_buffer();
	load_file_into_buffer(buffer, argv[1]);

	terminal_enable_raw_mode();

	// atexit setups (are called in reversed called order)
	if (atexit(home_screen) != 0) die("atexit in main");
	if (atexit(clear_screen) != 0) die("atexit in main");
	if (atexit(terminal_disable_raw_mode) != 0) die("atexit in main");

	clear_screen();
	home_screen();
	print_buffer(buffer);

	home_screen();
										
	cursor_t *cursor = init_cursor();

	unsigned char key;
	bool save_file = false;

	while (true) {
		ssize_t bytes_read = read(STDIN_FILENO, &key, 1);

		if (bytes_read == CHAR_ERROR) die("read in main");
		if (bytes_read == NO_CHAR) continue;
		if (key == CTRL_C) break;
		if (key == CTRL_S) {
			save_file = true;
			break;
		}
							  
		// handle arrow keys
		if (key == ESC) {
			// arrow keys are represented as 3 characters
			unsigned char buf[2];
			if ((bytes_read = read(STDIN_FILENO, buf, 2)) == -1) die("read after esc in main");

			// only escape was pressed
			if (bytes_read == NO_CHAR) continue;

			// handle arrow keys
			arrow_key_t arrow_key = get_arrow_key(buf[0], buf[1]);
			if (arrow_key) {
				move_with_arrows(arrow_key, cursor, buffer);
				continue;
			}
		}

		save_cursor_pos(cursor);

		if (key == ENTER) {
			bool should_redraw = create_new_line(buffer, cursor->y, cursor->x);

			if (should_redraw) {
				cursor->x = 0;
				set_cursor_pos(cursor);
				reprint_buffer_from_line(buffer, cursor->y);
			}

			restore_cursor_pos(cursor);

			move_down(cursor, buffer, MIN);
			continue;
		}

		line_t *line = get_line_from_buffer(buffer, cursor->y);

		if (key == BACKSPACE_1 || key == BACKSPACE_2) {
			if (cursor->x == 0 && cursor->y == 0) continue;

			if (cursor->x == 0) {
				move_up(cursor, buffer, MAX);
				save_cursor_pos(cursor);

				if (line->length == 0) {
					bool should_redraw = remove_empty_line(buffer, cursor->y+1);
					move_down(cursor, buffer, MIN);
					if (should_redraw) reprint_buffer_from_line(buffer, cursor->y);
				} else {
					remove_line_and_append_to_previous(buffer, cursor->y+1);
					cursor->x = 0;
					set_cursor_pos(cursor);
					reprint_buffer_from_line(buffer, cursor->y);
				}

				cursor->y = buffer->size;
				set_cursor_pos(cursor);
				if (write(STDOUT_FILENO, "\033[2K", 4) == -1) die("clear line write in main");

				restore_cursor_pos(cursor);
			} else {
				// TODO ADD DELETING LINE
				bool should_redraw = remove_character_from_line(line, cursor->x);
				if (should_redraw) {
					print_line(line, cursor->y);

					// cursor setup for last character
					cursor->x = line->length;
					set_cursor_pos(cursor);
				} else {
					// cursor setup for last character
					move_left(cursor, buffer);
				}

				// clear last character
				if (write(STDOUT_FILENO, " ", 1) == -1) die("clear last character in main");

				restore_cursor_pos(cursor);
				move_left(cursor, buffer);
			}
		} else {
			bool should_redraw = add_character_to_line(line, cursor->x, key);
			if (should_redraw) {
				print_line(line, cursor->y);
			} else {
				if (write(STDOUT_FILENO, &key, 1) == -1) die("print character in main");
			}

			restore_cursor_pos(cursor);
			move_right(cursor, buffer);
		}
	}

	if (save_file) {
		save_buffer_to_file(buffer, argv[1]);
		DEBUG("Saved file to %s\n", argv[1]);
	} else {
		DEBUG("Exited without saving\n");
	}

	free_buffer(buffer);
	free_cursor(cursor);
	return EXIT_SUCCESS;
}

void clear_screen(void) {
	if (write(STDOUT_FILENO, "\033[2J", 4) == -1) die("write in clear screen");
}

void home_screen(void) {
	if (write(STDOUT_FILENO, "\033[H", 3) == -1) die("write in home screen");
}
