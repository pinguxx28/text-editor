#pragma once
#include "line.h"
#include <stddef.h>
#include <stdbool.h>

#define BUFFER_CAPACITY_MULTIPLIER 2
#define DEFAULT_NUM_OF_BUFFER_LINES 256
#define MAX_NUM_OF_BUFFER_LINES 4096

typedef struct {
	line_t **lines;
	size_t size;
	size_t capacity; // TODO for later use
} buffer_t;

buffer_t *init_buffer(void);
void free_buffer(buffer_t *buffer);

line_t *get_line_from_buffer(buffer_t *buffer, size_t index);

void add_line_to_end_of_buffer(buffer_t *buffer, char *line);

void print_buffer(buffer_t *buffer);
void reprint_buffer_from_line(buffer_t *buffer, size_t line);

void load_file_into_buffer(buffer_t *buffer, char *pathname);
void save_buffer_to_file(buffer_t *buffer, char *pathname);

 create_new_line(buffer_t *buffer, size_t line, size_t column);

// new line functions
bool create_new_empty_line(buffer_t *buffer, size_t line);
void split_current_line(buffer_t *buffer, size_t line, size_t column);

// removin line functions
bool remove_empty_line(buffer_t *buffer, size_t line);
void remove_line_and_append_to_previous(buffer_t *buffer, size_t line);
