CC = clang

OBJ_DIR = .
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $< -o $@ -I.. -Wall -Wextra -Wpedantic -std=c89 -pthread

CPools: $(OBJS)

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -f $(OBJ_DIR)/*.o