#   

NAME = out

#SRC= claude_text.c
SRC= main.c helper_function.c raycasting.c event_handler.c texture.c collision.c minimap.c
OBJ=$(SRC:.c=.o)


all:$(NAME)
#libmlx42.a -Iinclude -lglfw
$(NAME): $(OBJ)
	$(CC) $(OBJ) -Lmlx -lmlx -framework OpenGL -framework AppKit -g -o $(NAME) #for mac compilation
	# $(CC) $(OBJ) -L./mlx -l:libmlx.a -L/usr/lib -Imlx_linux -lXext -lX11 -lm -lz -g -o $(NAME)	
%.o: %.c cube3d.h
	$(CC)  -Imlx -c -g $< -o $@ #for mac compilation
	# cc -Wall -Wextra -Werror -c $< -o $@

# -Wall -Wextra -Werror
clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY= re fclean clean all
