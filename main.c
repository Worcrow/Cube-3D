#include <mlx.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define MOVE_SPEED 0.1
#define ROTATION_SPEED 0.1

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
    t_player player;
} t_data;

// Simple map (1 represents walls, 0 represents empty space)
int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
}; 

void my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char *dst;

    if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT)
    {
        dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
        *(unsigned int*)dst = color;
    }
}

void draw_vertical_line(t_data *data, int x, int draw_start, int draw_end, int color)
{
    int y;

    y = 0;
    while (y < WINDOW_HEIGHT)
    {
        if (y < draw_start)
            my_mlx_pixel_put(data, x, y, 0x87CEEB); // Sky color
        else if (y >= draw_start && y <= draw_end)
            my_mlx_pixel_put(data, x, y, color);
        else
            my_mlx_pixel_put(data, x, y, 0x504030); // Floor color
        y++;
    }
}

int render_frame(t_data *data)
{
    int x;
    
    x = 0;
    while (x < WINDOW_WIDTH)
    {
        double camera_x = 2 * x / (double)WINDOW_WIDTH - 1;
        double ray_dir_x = data->player.dir_x + data->player.plane_x * camera_x;
        double ray_dir_y = data->player.dir_y + data->player.plane_y * camera_x;

        int map_x = (int)data->player.pos_x;
        int map_y = (int)data->player.pos_y;

        double delta_dist_x = fabs(1 / ray_dir_x);
        double delta_dist_y = fabs(1 / ray_dir_y);

        int step_x;
        int step_y;
        double side_dist_x;
        double side_dist_y;

        if (ray_dir_x < 0)
        {
            step_x = -1;
            side_dist_x = (data->player.pos_x - map_x) * delta_dist_x;
        }
        else
        {
            step_x = 1;
            side_dist_x = (map_x + 1.0 - data->player.pos_x) * delta_dist_x;
        }
        if (ray_dir_y < 0)
        {
            step_y = -1;
            side_dist_y = (data->player.pos_y - map_y) * delta_dist_y;
        }
        else
        {
            step_y = 1;
            side_dist_y = (map_y + 1.0 - data->player.pos_y) * delta_dist_y;
        }

        int hit = 0;
        int side;

        while (hit == 0)
        {
            if (side_dist_x < side_dist_y)
            {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0;
            }
            else
            {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1;
            }
            if (worldMap[map_x][map_y] > 0)
                hit = 1;
        }

        double wall_dist;
        if (side == 0)
            wall_dist = (map_x - data->player.pos_x + (1 - step_x) / 2) / ray_dir_x;
        else
            wall_dist = (map_y - data->player.pos_y + (1 - step_y) / 2) / ray_dir_y;

        int line_height = (int)(WINDOW_HEIGHT / wall_dist);
        int draw_start = -line_height / 2 + WINDOW_HEIGHT / 2;
        if (draw_start < 0)
            draw_start = 0;
        int draw_end = line_height / 2 + WINDOW_HEIGHT / 2;
        if (draw_end >= WINDOW_HEIGHT)
            draw_end = WINDOW_HEIGHT - 1;

        int color;
        if (side == 1)
            color = 0xFF0000;  // Red for vertical walls
        else
            color = 0xCC0000;  // Darker red for horizontal walls

        draw_vertical_line(data, x, draw_start, draw_end, color);
        x++;
    }
    mlx_put_image_to_window(data->mlx, data->win, data->img, 0, 0);
    return (0);
}

int key_press(int keycode, t_data *data)
{
    if (keycode == 53)  // ESC key
    {
        mlx_destroy_window(data->mlx, data->win);
        exit(0);
    }
    
    double oldDirX;
    double oldPlaneX;

    if (keycode == 13)  // W key
    {
        if (!worldMap[(int)(data->player.pos_x + data->player.dir_x * MOVE_SPEED)][(int)data->player.pos_y])
            data->player.pos_x += data->player.dir_x * MOVE_SPEED;
        if (!worldMap[(int)data->player.pos_x][(int)(data->player.pos_y + data->player.dir_y * MOVE_SPEED)])
            data->player.pos_y += data->player.dir_y * MOVE_SPEED;
    }
    if (keycode == 1)  // S key
    {
        if (!worldMap[(int)(data->player.pos_x - data->player.dir_x * MOVE_SPEED)][(int)data->player.pos_y])
            data->player.pos_x -= data->player.dir_x * MOVE_SPEED;
        if (!worldMap[(int)data->player.pos_x][(int)(data->player.pos_y - data->player.dir_y * MOVE_SPEED)])
            data->player.pos_y -= data->player.dir_y * MOVE_SPEED;
    }
    if (keycode == 0)  // A key - rotate left
    {
        oldDirX = data->player.dir_x;
        data->player.dir_x = data->player.dir_x * cos(ROTATION_SPEED) - data->player.dir_y * sin(ROTATION_SPEED);
        data->player.dir_y = oldDirX * sin(ROTATION_SPEED) + data->player.dir_y * cos(ROTATION_SPEED);
        oldPlaneX = data->player.plane_x;
        data->player.plane_x = data->player.plane_x * cos(ROTATION_SPEED) - data->player.plane_y * sin(ROTATION_SPEED);
        data->player.plane_y = oldPlaneX * sin(ROTATION_SPEED) + data->player.plane_y * cos(ROTATION_SPEED);
    }
    if (keycode == 2)  // D key - rotate right
    {
        oldDirX = data->player.dir_x;
        data->player.dir_x = data->player.dir_x * cos(-ROTATION_SPEED) - data->player.dir_y * sin(-ROTATION_SPEED);
        data->player.dir_y = oldDirX * sin(-ROTATION_SPEED) + data->player.dir_y * cos(-ROTATION_SPEED);
        oldPlaneX = data->player.plane_x;
        data->player.plane_x = data->player.plane_x * cos(-ROTATION_SPEED) - data->player.plane_y * sin(-ROTATION_SPEED);
        data->player.plane_y = oldPlaneX * sin(-ROTATION_SPEED) + data->player.plane_y * cos(-ROTATION_SPEED);
    }
    return (0);
}

int main(void)
{
    t_data data;

    data.mlx = mlx_init();
    data.win = mlx_new_window(data.mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "Raycaster");
    data.img = mlx_new_image(data.mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
    data.addr = mlx_get_data_addr(data.img, &data.bits_per_pixel, &data.line_length, &data.endian);

    // Initialize player position and direction
    data.player.pos_x = 2;
    data.player.pos_y = 2;
    data.player.dir_x = -1;
    data.player.dir_y = 0;
    data.player.plane_x = 0;
    data.player.plane_y = 0.66;

    mlx_hook(data.win, 2, 1L<<0, key_press, &data);
    mlx_loop_hook(data.mlx, render_frame, &data);
    mlx_loop(data.mlx);

    return (0);
}
