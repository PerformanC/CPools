CC := clang

test:
	$(CC) tests/*.c -I.. -Wall -Wextra -Wpedantic -std=c89 -pthread