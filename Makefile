#
# ChatGPT generated code 
#

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror -g

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, dist/%.o, $(SRC))

OUT = main
DIST_DIR = dist

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $(OUT)

# Compile .c -> dist/.o
dist/%.o: src/%.c | $(DIST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure dist/ exists
$(DIST_DIR):
	mkdir -p $(DIST_DIR)

clean:
	rm -rf $(DIST_DIR) $(OUT)

