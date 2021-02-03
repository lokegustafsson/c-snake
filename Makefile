asan:
	gcc -lSDL2 -g -Wall -Wextra -fsanitize=address,undefined snake.c circular_buffer.c && ./a.out

run:
	gcc -lSDL2 -O2 snake.c circular_buffer.c && ./a.out
