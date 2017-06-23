all:
	gcc -Wall -Wextra -pthread yx2.c -o yx2

test:
	./yx2 1000000000 4

format:
	 clang-format -i -style=LLVM *.c && astyle --style=linux *.c
