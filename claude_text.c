#include "mlx/mlx.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define TEX_WIDTH 64
#define TEX_HEIGHT 64
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define MOVE_SPEED 5.0
#define ROT_SPEED 3.0

typedef struct s_texture {
    void    *img;
    char    *addr;
    int     bits_per_pixel;
    int     line_length;
    int     endian;
} t_texture;

typedef struct s_data {
    void    *mlx;
    void    *win;
    void    *img;
    char    *addr;
    int     bits_per_pixel;
    int     line_length;
    int     endian;
    double  pos_x;
    double  pos_y;
    double  dir_x;
    double  dir_y;
    double  plane_x;
    double  plane_y;
    double  time;
    double  old_time;
    t_texture textures[8];
} t_data;

// World map (same as original)
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

    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
    {
        dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
        *(unsigned int*)dst = color;
    }
}

unsigned int get_texture_color(t_texture *tex, int x, int y)
{
    char *dst;
    
    dst = tex->addr + (y * tex->line_length + x * (tex->bits_per_pixel / 8));
    return (*(unsigned int*)dst);
}

void load_texture(t_data *data, int index, char *path)
{
    int width;
    int height;
    
    data->textures[index].img = mlx_xpm_file_to_image(data->mlx, path, &width, &height);
    if (!data->textures[index].img)
    {
        printf("Error loading texture %s\n", path);
        exit(1);
    }
    data->textures[index].addr = mlx_get_data_addr(data->textures[index].img,
        &data->textures[index].bits_per_pixel,
        &data->textures[index].line_length,
        &data->textures[index].endian);
}

void draw_textured_line(t_data *data, int x, int tex_num, 
                       double wall_x, double perp_wall_dist,
                       int draw_start, int draw_end, int side)
{
    int y;
    t_texture *tex = &data->textures[tex_num];
    
    // Calculate texture X coordinate
    int tex_x = (int)(wall_x * TEX_WIDTH);
    if (side == 0 && data->dir_x > 0) tex_x = TEX_WIDTH - tex_x - 1;
    if (side == 1 && data->dir_y < 0) tex_x = TEX_WIDTH - tex_x - 1;

    // Calculate line height
    int line_height = (int)(SCREEN_HEIGHT / perp_wall_dist);
    
    // Calculate texture step and starting position
    double step = 1.0 * TEX_HEIGHT / line_height;
    double tex_pos = (draw_start - SCREEN_HEIGHT / 2 + line_height / 2) * step;
    
    y = draw_start;
    while (y < draw_end)
    {
        int tex_y = (int)tex_pos & (TEX_HEIGHT - 1);
        tex_pos += step;
        
        unsigned int color = get_texture_color(tex, tex_x, tex_y);
        if (side == 1)
            color = (color >> 1) & 8355711; // Make color darker for y-sides
            
        my_mlx_pixel_put(data, x, y, color);
        y++;
    }
}

int render_frame(t_data *data)
{
    int x;
    
    x = 0;
    while (x < SCREEN_WIDTH)
    {
        double camera_x = 2 * x / (double)SCREEN_WIDTH - 1;
        double ray_dir_x = data->dir_x + data->plane_x * camera_x;
        double ray_dir_y = data->dir_y + data->plane_y * camera_x;

        int map_x = (int)data->pos_x;
        int map_y = (int)data->pos_y;

        double delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1 / ray_dir_x);
        double delta_dist_y = (ray_dir_y == 0) ? 1e30 : fabs(1 / ray_dir_y);

        double side_dist_x;
        double side_dist_y;
        int step_x;
        int step_y;
        int hit = 0;
        int side;

        if (ray_dir_x < 0)
        {
            step_x = -1;
            side_dist_x = (data->pos_x - map_x) * delta_dist_x;
        }
        else
        {
            step_x = 1;
            side_dist_x = (map_x + 1.0 - data->pos_x) * delta_dist_x;
        }
        if (ray_dir_y < 0)
        {
            step_y = -1;
            side_dist_y = (data->pos_y - map_y) * delta_dist_y;
        }
        else
        {
            step_y = 1;
            side_dist_y = (map_y + 1.0 - data->pos_y) * delta_dist_y;
        }

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

        double perp_wall_dist;
        if (side == 0)
            perp_wall_dist = (side_dist_x - delta_dist_x);
        else
            perp_wall_dist = (side_dist_y - delta_dist_y);

        int line_height = (int)(SCREEN_HEIGHT / perp_wall_dist);
        int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_start < 0)
            draw_start = 0;
        int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_end >= SCREEN_HEIGHT)
            draw_end = SCREEN_HEIGHT - 1;

        // Calculate wall X
        double wall_x;
        if (side == 0)
            wall_x = data->pos_y + perp_wall_dist * ray_dir_y;
        else
            wall_x = data->pos_x + perp_wall_dist * ray_dir_x;
        wall_x -= floor(wall_x);

        int tex_num = worldMap[map_x][map_y] - 1;
        if (tex_num >= 8)
            tex_num = 0;

        draw_textured_line(data, x, tex_num, wall_x, perp_wall_dist, 
                          draw_start, draw_end, side);
        x++;
    }
    mlx_put_image_to_window(data->mlx, data->win, data->img, 0, 0);
    return (0);
}

int key_press(int keycode, t_data *data)
{
    double move_speed = 0.05;
    double rot_speed = 0.05;

    if (keycode == 53)  // ESC
        exit(0);
    if (keycode == 13)  // W
    {
        if (!worldMap[(int)(data->pos_x + data->dir_x * move_speed)][(int)data->pos_y])
            data->pos_x += data->dir_x * move_speed;
        if (!worldMap[(int)data->pos_x][(int)(data->pos_y + data->dir_y * move_speed)])
            data->pos_y += data->dir_y * move_speed;
    }
    if (keycode == 1)   // S
    {
        if (!worldMap[(int)(data->pos_x - data->dir_x * move_speed)][(int)data->pos_y])
            data->pos_x -= data->dir_x * move_speed;
        if (!worldMap[(int)data->pos_x][(int)(data->pos_y - data->dir_y * move_speed)])
            data->pos_y -= data->dir_y * move_speed;
    }
    if (keycode == 0)   // A - rotate left
    {
        double old_dir_x = data->dir_x;
        data->dir_x = data->dir_x * cos(rot_speed) - data->dir_y * sin(rot_speed);
        data->dir_y = old_dir_x * sin(rot_speed) + data->dir_y * cos(rot_speed);
        double old_plane_x = data->plane_x;
        data->plane_x = data->plane_x * cos(rot_speed) - data->plane_y * sin(rot_speed);
        data->plane_y = old_plane_x * sin(rot_speed) + data->plane_y * cos(rot_speed);
    }
    if (keycode == 2)   // D - rotate right
    {
        double old_dir_x = data->dir_x;
        data->dir_x = data->dir_x * cos(-rot_speed) - data->dir_y * sin(-rot_speed);
        data->dir_y = old_dir_x * sin(-rot_speed) + data->dir_y * cos(-rot_speed);
        double old_plane_x = data->plane_x;
        data->plane_x = data->plane_x * cos(-rot_speed) - data->plane_y * sin(-rot_speed);
        data->plane_y = old_plane_x * sin(-rot_speed) + data->plane_y * cos(-rot_speed);
    }
    mlx_destroy_image(data->mlx, data->img);
    data->img = mlx_new_image(data->mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
    data->addr = mlx_get_data_addr(data->img, &data->bits_per_pixel, &data->line_length, &data->endian);
    render_frame(data);
    return (0);
}

int main(void)
{
    t_data data;

    data.mlx = mlx_init();
    data.win = mlx_new_window(data.mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "MLX Raycaster");
    data.img = mlx_new_image(data.mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
    data.addr = mlx_get_data_addr(data.img, &data.bits_per_pixel,
                                 &data.line_length, &data.endian);

    // Load textures
    load_texture(&data, 0, "textures/eagle.xpm");
    load_texture(&data, 1, "textures/redbrick.xpm");
    load_texture(&data, 2, "textures/purplestone.xpm");
    load_texture(&data, 3, "textures/greystone.xpm");
    load_texture(&data, 4, "textures/bluestone.xpm");
    load_texture(&data, 5, "textures/mossy.xpm");
    load_texture(&data, 6, "textures/wood.xpm");
    load_texture(&data, 7, "textures/colorstone.xpm");

    // Initialize player position and direction
    data.pos_x = 22.0;
    data.pos_y = 11.5;
    data.dir_x = -1.0;
    data.dir_y = 0.0;
    data.plane_x = 0.0;
    data.plane_y = 0.66;

    mlx_hook(data.win, 2, 1L<<0, key_press, &data);
    mlx_loop_hook(data.mlx, render_frame, &data);
    mlx_loop(data.mlx);

    return (0);
}