
#ifndef __CUBE3D_H__
# define __CUBE3D_H__

# include "mlx/mlx.h"
# include <math.h>
# include <stdio.h>
# include <stdlib.h>


# define WINDOW_WIDTH 1024
# define WINDOW_HEIGHT 512
# define MAP_WIDTH 24
# define MAP_HEIGHT 24
# define MOVE_SPEED 0.1
# define ROTATION_SPEED 0.1

typedef struct s_player {
    double pos_x;
    double pos_y;
    double dir_x;
    double dir_y;
    double plane_x;
    double plane_y;
} t_player;

typedef struct s_data {
    void *mlx;
    void *win;
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
    int **worldMap;
    t_player player;
} t_data;

int     render_frame(t_data *data);
int     key_press(int keycode, t_data *data);
void    my_mlx_pixel_put(t_data *data, int x, int y, int color);
void    draw_vertical_line(t_data *data, int x, int draw_start, int draw_end, int color);
void    create_worldMap(t_data *data);




#endif