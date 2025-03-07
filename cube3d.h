#ifndef __CUBE3D_H__
# define __CUBE3D_H__

# include "mlx/mlx.h"
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <string.h>


# define WINDOW_WIDTH 1024
# define WINDOW_HEIGHT 512
# define TEXTURE_WIDTH 64
# define TEXTURE_HEIGHT 64
# define MAP_WIDTH 24
# define MAP_HEIGHT 24
# define MOVE_SPEED 0.1
# define ROTATION_SPEED 0.1
# define COLLISION_BUFFER 0.2  // Buffer distance from walls

// Define texture indices for clarity
# define TEX_NORTH 0
# define TEX_SOUTH 1
# define TEX_EAST 2
# define TEX_WEST 3
# define TEX_BLUESTONE 4
# define TEX_MOSSY 5
# define TEX_BARREL 6
# define TEX_GREENLIGHT 7

#define MINIMAP_WIDTH 150
#define MINIMAP_HEIGHT 150
#define MINIMAP_SCALE_X (MINIMAP_WIDTH / MAP_WIDTH)
#define MINIMAP_SCALE_Y (MINIMAP_HEIGHT / MAP_HEIGHT)
#define MINIMAP_OFFSET_X 10
#define MINIMAP_OFFSET_Y 10

typedef struct s_player {
    double pos_x;
    double pos_y;
    double dir_x;
    double dir_y;
    double plane_x;
    double plane_y;
} t_player;

typedef struct s_texture {
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
} t_texture;

typedef struct s_data {
    void *mlx;
    void *win;
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
    int **worldMap;
    t_texture textures[9];  // Keep textures for walls
    t_player player;
} t_data;

int             render_frame(t_data *data);
int             key_press(int keycode, t_data *data);
void            my_mlx_pixel_put(t_data *data, int x, int y, int color);
void            draw_vertical_line(t_data *data, int x, int draw_start, int draw_end, int color);
void            create_worldMap(t_data *data);
void            load_texture(t_data *data);
unsigned int    get_texture_color(t_texture *tex, int y, int x);
int             check_wall_collision(t_data *data, double new_x, double new_y);
void            draw_minimap(t_data *data);
int             mouse_move(int x, int y, t_data *data);
int             mouse_enter(t_data *data);
int             mouse_leave(t_data *data);

#endif