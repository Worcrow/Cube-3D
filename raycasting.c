#include "cube3d.h"

void floor_ceil_render(t_data *data)
{
    size_t y;
    size_t x;
    unsigned int floorColor;
    unsigned int ceilColor;

    y = 0;
    while (y < WINDOW_HEIGHT)
    {
        floorColor = 0x1F1F1F;
        ceilColor = 0xDC6400; 
        x = 0;
        while (x < WINDOW_WIDTH)
        {
            my_mlx_pixel_put(data, x, y, ceilColor);
            my_mlx_pixel_put(data, x, WINDOW_HEIGHT - y - 1, floorColor);
            x++;
        }
        y++;
    }
}

int render_frame(t_data *data)
{
    int x;
    
    x = 0;
    floor_ceil_render(data);
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
            if (data->worldMap[map_x][map_y] > 0)
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

        // int color;
        // if (side == 1)
        //     color = 0xFF0000;  // Red for vertical walls
        // else
        //     color = 0xCC0000;  // Darker red for horizontal walls

        // draw_vertical_line(data, x, draw_start, draw_end, color);

        // Determine which texture to use based on wall orientation and side
        int textNUM;
        if (side == 0) // x-axis wall (east/west facing)
        {
            if (ray_dir_x > 0) // west facing wall
                textNUM = 0;    // eagle texture
            else               // east facing wall
                textNUM = 1;    // redbrick texture
        }
        else          // y-axis wall (north/south facing)
        {
            if (ray_dir_y > 0) // north facing wall
                textNUM = 2;    // purplestone texture
            else               // south facing wall
                textNUM = 3;    // greystone texture
        }

        // Calculate wall X coordinate
        double wall_x;
        if (side == 0)
            wall_x = data->player.pos_y + wall_dist * ray_dir_y;
        else
            wall_x = data->player.pos_x + wall_dist * ray_dir_x;
        wall_x -= floor(wall_x);
        
        // x coordinate on the texture
        int textX = (int)(wall_x * (double)TEXTURE_HEIGHT);
        if (side == 0 && ray_dir_x > 0)
            textX = TEXTURE_WIDTH - textX - 1;
        if (side == 1 && ray_dir_y < 0)
            textX = TEXTURE_WIDTH - textX - 1;

        // y coordinate on the texture
        // How much to increase the texture coordinate per screen pixel
        double step = 1.0 * TEXTURE_HEIGHT / line_height;
        double text_pos = (draw_start - WINDOW_HEIGHT / 2 + line_height / 2) * step;
        for (int y = draw_start; y < draw_end; y++)
        {
            // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
            int textY = (int)text_pos & (TEXTURE_HEIGHT - 1);
            text_pos += step;
            unsigned int color = get_texture_color(data->textures + textNUM, textY, textX);
            if (side == 1)
                color = (color >> 1) & 8355711;
            my_mlx_pixel_put(data, x, y, color);
        }

        x++;
    }
    mlx_put_image_to_window(data->mlx, data->win, data->img, 0, 0);
    return (0);
}