#pragma once
#include <stddef.h>
#include <stdbool.h>

#define MAX_LINE_CAPACITY 1024
#define DEFAULT_LINE_CAPACITY 80

// how much bigger the capacity is than the length at initiailization
// also how much bigger the capacity gets when we add over capacity
// characters to the line
#define LINE_CAPACITY_MULTIPLIER 2

typedef struct {
	char *text;
	size_t length;
	size_t capacity;
} line_t;

line_t *init_line(char *str);
void free_line(line_t *line);

void print_line_here(line_t *line);
void print_line(line_t *line, size_t y);
void move_characters_in_line(line_t *line, size_t position);
// bool dictates if we have to redraw the entire line?
bool add_character_to_line(line_t *line, size_t index, char c);
bool remove_character_from_line(line_t *line, size_t index);
