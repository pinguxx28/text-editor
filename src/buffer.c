#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "die.h"
#include "line.h"
#include "debug.h"

buffer_t *init_buffer(void) {
	buffer_t *buffer = malloc(sizeof(buffer_t));
	if (buffer == NULL) die("malloc in init_buffer");

	buffer->lines = malloc(sizeof(line_t) * DEFAULT_NUM_OF_BUFFER_LINES);
	if (buffer->lines == NULL) die("malloc in init_buffer (lines)");

	buffer->size = 0;
	buffer->capacity = DEFAULT_NUM_OF_BUFFER_LINES;
	return buffer;
} void free_buffer(buffer_t *buffer) {
	if (buffer == NULL) die("buffer is null in free_buffer");
	if (buffer->lines == NULL) die("buffer lines is null in free_buffer");

	for (size_t i = 0; i < buffer->size; i++) {
		if (buffer->lines[i] == NULL) {
			die("one buffer line is null in free_buffer");
		}

		free_line(buffer->lines[i]);
		buffer->lines[i] = NULL;
	}

	free(buffer->lines);
	buffer->lines = NULL;

	free(buffer);
	buffer = NULL;
}

line_t *get_line_from_buffer(buffer_t *buffer, size_t index) {
	if (buffer == NULL) die("buffer is null in get_line");
	if (buffer->lines == NULL) die("buffer lines is null in get_line");
	if (index >= buffer->size) die("index is bigger than buffer size in get_line");
	if (index >= buffer->capacity) die("index is bigger than buffer capacity in get_line");
	if (buffer->lines[index] == NULL) die("buffer->lines[index] is null in get_line");

	return buffer->lines[index];
}

void add_line_to_end_of_buffer(buffer_t *buffer, char *line) {
	if (line == NULL) die("line is null in add_line");
	if (buffer == NULL) die("buffer is null in add_line");
	if (buffer->lines == NULL) die("buffer lines is null in add_line");
	if (buffer->size + 1 >= buffer->capacity) die("tried to create too many buffer lines (change in the future)");

	buffer->lines[buffer->size] = init_line(line);
	buffer->size++;
}

void print_buffer(buffer_t *buffer) {
	if (buffer == NULL) die("buffer is null in print_buffer");
	if (buffer->lines == NULL) die("buffer lines is null in print_buffer");

	for (size_t i = 0; i < buffer->size; i++) {
		if (buffer->lines[i] == NULL) die("buffer->lines[i] is null in print_buffer");

		print_line_here(buffer->lines[i]);
		if (write(STDOUT_FILENO, "\033[1E", 4) == -1) die("write in print_buffer");
	}
}

void load_file_into_buffer(buffer_t *buffer, char *pathname) {
	if (buffer == NULL) die("buffer is null in load_into_buffer");
	if (pathname == NULL) die("pathname is null in load_into_buffer");

	FILE *file = fopen(pathname, "r");
	if (file == NULL) die("fopen in load_into_buffer");

	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	size_t line_num = 0;

	while ((nread = getline(&line, &len, file)) != -1) {
		if (line[nread - 1] == '\n') {
			line[nread - 1] = '\0';
		}

		add_line_to_end_of_buffer(buffer, line);

		line_num++;
		if (line_num >= buffer->capacity) {
			buffer->capacity *= BUFFER_CAPACITY_MULTIPLIER;

			if (buffer->capacity > MAX_NUM_OF_BUFFER_LINES) {
				buffer->capacity = MAX_NUM_OF_BUFFER_LINES;
			}

			buffer->lines = realloc(buffer->lines, sizeof(line_t) * buffer->capacity);
			if (buffer->lines == NULL) die("buffer lines is null after realloc in load_into_buffer");
		}
	}

	free(line);
	if (fclose(file) == EOF) die("fclose in load_into_buffer");
}

void save_buffer_to_file(buffer_t *buffer, char *pathname) {
	if (buffer == NULL) die("buffer is null in save_buffer_to_file");
	if (pathname == NULL) die("pathname is null in save_buffer_to_file");

	FILE *file = fopen(pathname, "w");
	if (file == NULL) die("fopen in save_buffer_into_file");

	for (size_t i = 0; i < buffer->size; i++) {
		line_t *line = get_line_from_buffer(buffer, i);

		if (line->length > 0) {
			if (fwrite(line->text, sizeof(char), line->length, file) == 0) die("fwrite in save_buffer_to_file");
		}
		if (fwrite("\n", sizeof(char), 1, file) == 0) die("fwrite in save_buffer_to_file");
	}

	if (fclose(file) == EOF) die("fclose in save_buffer_to_file");
}

bool create_new_empty_line(buffer_t *buffer, size_t row) {
	if (buffer == NULL) die("buffer is null in create_new_empty_line");
	if (buffer->lines == NULL) die("buffer lines is null in create_new_empty_line");
	if (buffer->size >= buffer->capacity) die("buffer size is bigger than buffer capacity in create_new_empty_line");
	if (row >= buffer->capacity) die("row is bigger than buffer capacity in create_new_empty_line");
	if (row >= buffer->size) die("row is bigger than buffer size in create_new_empty_line");
	if (buffer->lines[row] == NULL) die("buffer->lines[row] is null in create_new_empty_line");

	bool should_redraw = false;

	if (row != buffer->size) {
		memmove(
			&buffer->lines[row + 1],
			&buffer->lines[row],
			(buffer->size - row) * sizeof(line_t));
		should_redraw = true;
	}

	buffer->size++;
	return should_redraw;
}

void split_current_line(buffer_t *buffer, size_t row, size_t column) {
	if (buffer == NULL) die("buffer is null in split_current_line_line");
	if (buffer->lines == NULL) die("buffer lines is null in split_current_line_line");
	if (buffer->size >= buffer->capacity) die("buffer size is bigger than buffer capacity in split_current_line_line");
	if (row >= buffer->capacity) die("row is bigger than buffer capacity in split_current_line_line");
	if (row >= buffer->size) die("row is bigger than buffer size in split_current_line_line");
	if (buffer->lines[row] == NULL) die("buffer->lines[row] is null in split_current_line_line");

	line_t *line = get_line_from_buffer(buffer, row);

	char *line_start = malloc(column + 1);
	if (line_start == NULL) die("line_start malloc in create_new_line");
	strncpy(line_start, line->text, column);
	line_start[column] = '\0';
	
	char *line_end = malloc(line->length - column + 1);
	if (line_end == NULL)   die("lien_end malloc in create_new_line");
	strncpy(line_end, line->text + column, line->length - column);
	line_end[line->length - column] = '\0';

	if (row != buffer->size) {
		memmove(
			&buffer->lines[row + 1],
			&buffer->lines[row],
			(buffer->size - row) * sizeof(line_t));
	}

	free_line(buffer->lines[row]);
	buffer->lines[row] = init_line(line_start);
	// Don't free row + 1 because row + 1 == row this would lead to a double free
	// free_line(buffer->lines[row + 1]);
	buffer->lines[row + 1] = init_line(line_end);

	buffer->size++;

	free(line_start);
	free(line_end);
}

bool remove_empty_line(buffer_t *buffer, size_t line) {
	if (buffer == NULL) die("buffer is null in remove_line");
	if (buffer->lines == NULL) die("buffer lines is null in remove_line");
	if (buffer->size <= 0) die("buffer size is less or equal to 0 in remove_line");
	if (line >= buffer->size) die("line is bigger than buffer size in remove_line");
	if (line >= buffer->capacity) die("line is bigger than buffer capacity in remove_line");
	if (buffer->lines[line] == NULL) die("buffer->lines[line] is null");

	bool should_redraw = false;

	free_line(buffer->lines[line]);

	if (line != buffer->size - 1) {
		memmove(
			&buffer->lines[line],
			&buffer->lines[line + 1],
			(buffer->size - line) * sizeof(line_t));
		should_redraw = true;
	}

	buffer->size--;

	return should_redraw;
}

void remove_line_and_append_to_previous(buffer_t *buffer, size_t line) {
	if (buffer == NULL) die("buffer is null in remove_line_and_append_to_previous");
	if (buffer->lines == NULL) die("buffer lines is null in remove_line_and_append_to_previous");
	if (line == 0) die("line is 0 in remove_line_and_append_to_previous");
	if (line >= buffer->size) die("line is bigger than buffer size in remove_line_and_append_to_previous");
	if (line >= buffer->capacity) die("line is bigger than buffer capacity in remove_line_and_append_to_previous");
	if (buffer->lines[line] == NULL) die("buffer->lines[line] is null in remove_line_and_append_to_previous");
	if (buffer->lines[line - 1] == NULL) die("buffer->lines[line-1] is null in remove_line_and_append_to_previous");

	line_t *current_line = get_line_from_buffer(buffer, line);
	line_t *previous_line = get_line_from_buffer(buffer, line - 1);
	const size_t new_len = current_line->length + previous_line->length;
	const size_t new_capacity = new_len * 2;

	previous_line->text = realloc(previous_line->text, sizeof(char) * (int)new_capacity);
	if (previous_line->text == NULL) die("realloc in remove_line_and_append_to_previous");

	for (size_t i = previous_line->length; i < new_len; i++) {
		previous_line->text[i] = current_line->text[i - previous_line->length];
	}

	previous_line->length = new_len;
	previous_line->capacity = new_capacity;

	free_line(current_line);

	if (line != buffer->size - 1) {
		memmove(
			&buffer->lines[line],
			&buffer->lines[line + 1],
			(buffer->size - line) * sizeof(line_t));
	}
	buffer->size--;
}

void reprint_buffer_from_line(buffer_t *buffer, size_t line) {
	if (buffer == NULL) die("buffer is null in redraw_lines");
	if (buffer->lines == NULL) die("buffer lines is null in redraw_lines");
	if (line >= buffer->size) die("line is bigger than buffer size in redraw_lines");
	if (line >= buffer->capacity) die("line is bigger than buffer capacity in redraw_lines");

	// OBSERVATE we are assuming we start at line.
	for (size_t i = line; i < buffer->size; i++ ) {
		if (buffer->lines[i] == NULL) die("buffer->lines[i] is null in redraw_lines");
		if (write(STDOUT_FILENO, "\033[2K", 4) == -1) die("write clear line in redraw_lines");
		print_line_here(buffer->lines[i]);
		if (write(STDOUT_FILENO, "\033[1E", 4) == -1) die("write newline in redraw_lines");
	}
}
