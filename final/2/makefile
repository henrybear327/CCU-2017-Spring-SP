all:
	gcc -Wall -Wextra -pthread mypipe.c -o mypipe

test:
	./mypipe cat mypipe.c wc -c

format:
	 clang-format -i -style=LLVM *.c && astyle --style=linux *.c
