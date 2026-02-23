#pragma once
#include "line.h"
#include <stddef.h>
#include <stdbool.h>

#define MAX_BUFFER_LINES 16

typedef struct {
	line_t **lines;
	size_t size;
	size_t capacity; // TODO for later use
} buffer_t;

buffer_t *init_buffer(void);
void free_buffer(buffer_t *buffer);

void add_line(buffer_t *buffer, char *line);
line_t *get_line(buffer_t *buffer, size_t index);
void print_buffer(buffer_t *buffer);

void load_into_buffer(buffer_t *buffer, char *pathname);
void save_buffer_to_file(buffer_t *buffer, char *pathname);

bool create_new_line(buffer_t *buffer, size_t line, size_t column);
bool remove_empty_line(buffer_t *buffer, size_t line);
void remove_line_and_append_to_previous(buffer_t *buffer, size_t line);
void redraw_lines(buffer_t *buffer, size_t line);
