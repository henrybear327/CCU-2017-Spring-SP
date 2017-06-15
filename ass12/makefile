all:
	gcc -Wall -Wextra -O3 -pthread main.c -o pi

format:
	clang-format -i -style=LLVM *.c 
	astyle --style=linux *.c
