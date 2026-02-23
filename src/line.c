#include "line.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "die.h"

line_t *init_line(char *str) {
	line_t *line = malloc(sizeof(line_t));
	if (line == NULL) die("malloc line from init_line");

	line->length = strnlen(str, MAX_LINE_CAPACITY);

	if (line->length != 0) {
		line->capacity = line->length * LINE_CAPACITY_MULTIPLIER;

		if (line->capacity > MAX_LINE_CAPACITY) {
			line->capacity = MAX_LINE_CAPACITY;
		}
	} else {
		line->capacity = DEFAULT_LINE_CAPACITY;
	}

	line->text = malloc(sizeof(char) * line->capacity);
	if (line->text == NULL) die("malloc text from init_line");

	strncpy(line->text, str, line->capacity - 1);
	// OBS we don't set a null terminator because we always print length bytes
	return line;
}

void free_line(line_t *line) {
	if (line == NULL) die("line is null in free_line");
	if (line->text == NULL) die("line text is null in free_line");

	free(line->text);
	line->text = NULL;

	free(line);
	line = NULL;
}

void print_line_here(line_t *line) {
	if (line == NULL) die("line is null in print_line_here");
	if (line->text == NULL) die("line text is null in print_line_here");

	if (write(STDOUT_FILENO, line->text, line->length) == -1) {
		die("write in print_line_here");
	}
	
	// for (size_t i = 0; i < line->length; i++) {
		// if (write(STDOUT_FILENO, &line->text[i], 1) == -1) {
		// 	die("write in print_line_here");
		// }
	// }
}

void print_line(line_t *line, size_t y) {
	if (line == NULL) die("line is null in print_line");
	if (line->text == NULL) die("line text is null in print_line");

	char buf[32];

	// set position
    int len = snprintf(buf, sizeof(buf), "\033[%ld;%dH", y + 1, 1);
    if (len < 0) die("snprintf in print_line");
    if (write(STDOUT_FILENO, buf, len) == -1) {
        die("write in print_line");
	}

	// write
	print_line_here(line);
}

bool add_character_to_line(line_t *line, size_t index, char c) {
	if (line == NULL) die("line is null in add_character_to_line");
	if (line->text == NULL) die("line text is null in add_character_to_line");
	if (index > line->length) die("index bigger than line length in add_character_to_line");
	if (index > line->capacity) die("index bigger than line capacity in add_character_to_line");
	if (line->length > line->capacity) die("line length bigger than line capacity in add_character_to_line");

	if (index == line->capacity) {
		line->capacity *= LINE_CAPACITY_MULTIPLIER;

		if (line->capacity > MAX_LINE_CAPACITY) {
			line->capacity = MAX_LINE_CAPACITY;
		}

		line->text = realloc(line->text, sizeof(char) * line->capacity);
		if (line->text == NULL) die("realloc in set_line");
	}

	bool rewrite_line = false;

	// in the middle of the line
	if (index != line->length) {
		memmove(
			&line->text[index + 1],
			&line->text[index],
			line->length - index);
		rewrite_line = true;
	}

	line->text[index] = c;
	line->length++;

	return rewrite_line;
}

bool remove_character_from_line(line_t *line, size_t index) {
	if (line == NULL) die("line is null in remove_character_from_line");
	if (line->text == NULL) die("line text is null in remove_character_from_line");
	if (index > line->length) die("index bigger than line length in remove_character_from_line");
	if (index > line->capacity) die("index bigger than line capacity in remove_character_from_line");
	if (line->length > line->capacity) die("line length bigger than line capacity in remove_character_from_line");
	if (line->length == 0) die("line length 0 in remove_character_from_line");

	bool rewrite_line = false;

	if (index != line->length) {
		memmove(
			&line->text[index - 1],
			&line->text[index],
			line->length - index);
		rewrite_line = true;
	}

	line->length--;

	return rewrite_line;
}
