
#   

NAME = out

SRC= main.c
OBJ=$(SRC:.c=.o)


all:$(NAME)
#libmlx42.a -Iinclude -lglfw
$(NAME): $(OBJ)
	$(CC) $(OBJ) -Lmlx -lmlx -framework OpenGL -framework AppKit -g -o $(NAME)

%.o: %.c cube.h
	$(CC)  -Imlx -c -g $< -o $@

# -Wall -Wextra -Werror
clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY= re fclean clean all
