all:
	gcc -Wall -Wextra -pthread -O2 yx2.c -o yx2

test_1:
	./yx2 1000000000 1

test_2:
	./yx2 1000000000 2

test_4:
	./yx2 1000000000 4

test_8:
	./yx2 1000000000 8

format:
	 clang-format -i -style=LLVM *.c && astyle --style=linux *.c
