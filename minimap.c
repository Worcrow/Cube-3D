#include "cube3d.h"

void draw_minimap(t_data *data)
{
    int map_x = 0;
    int map_y = 0;
    int minimap_x, minimap_y;
    int color;

    // Loop through the map
    while (map_x < MAP_WIDTH)
    {
        map_y = 0;
        while (map_y < MAP_HEIGHT)
        {
            if (data->worldMap[map_x][map_y] > 0)
                color = 0xDC6400; 
            else
                color = 0x000000; 

            // Fix the coordinate mapping - swap x and y to match the player's perspective
            minimap_x = (int)(map_y * MINIMAP_SCALE_X) + MINIMAP_OFFSET_X;
            minimap_y = (int)(map_x * MINIMAP_SCALE_Y) + MINIMAP_OFFSET_Y;

            int i = 0;
            while (i < MINIMAP_SCALE_X)
            {
                int j = 0;
                while (j < MINIMAP_SCALE_Y)  // Use MINIMAP_SCALE_Y for consistency
                {
                    my_mlx_pixel_put(data, minimap_x + i, minimap_y + j, color);
                    j++;
                }
                i++;
            }
            map_y++;
        }
        map_x++;
    }

    // Fix player position on minimap - swap x and y coordinates
    int player_minimap_x = (int)(data->player.pos_y * MINIMAP_SCALE_X) + MINIMAP_OFFSET_X;
    int player_minimap_y = (int)(data->player.pos_x * MINIMAP_SCALE_Y) + MINIMAP_OFFSET_Y;
    
    // Draw player as a larger dot (10x10 pixels instead of 5x5)
    for (int i = -4; i <= 3; i++) {
        for (int j = -4; j <= 3; j++) {
            my_mlx_pixel_put(data, player_minimap_x + i, player_minimap_y + j, 0xFF0000);
        }
    }
    
    // Draw direction indicator (a line showing which way the player is facing)
    int dir_length = 8; // Increased from 5 to 8 for better visibility
    int dir_end_x = player_minimap_x + (int)(data->player.dir_y * dir_length);
    int dir_end_y = player_minimap_y + (int)(data->player.dir_x * dir_length);
    
    // Draw the direction line
    double dx = dir_end_x - player_minimap_x;
    double dy = dir_end_y - player_minimap_y;
    double steps = fmax(fabs(dx), fabs(dy));
    
    if (steps > 0) {
        double x_inc = dx / steps;
        double y_inc = dy / steps;
        double x = player_minimap_x;
        double y = player_minimap_y;
        
        for (int i = 0; i <= steps; i++) {
            my_mlx_pixel_put(data, (int)x, (int)y, 0xFFFF00);  // Yellow direction line
            x += x_inc;
            y += y_inc;
        }
    }
    
    // Draw FOV visualization
    // Calculate FOV edges based on player's direction and plane vectors
    double left_ray_x = data->player.dir_y - data->player.plane_y;
    double left_ray_y = data->player.dir_x - data->player.plane_x;
    double right_ray_x = data->player.dir_y + data->player.plane_y;
    double right_ray_y = data->player.dir_x + data->player.plane_x;
    
    // Normalize and scale the rays
    double left_length = sqrt(left_ray_x * left_ray_x + left_ray_y * left_ray_y);
    double right_length = sqrt(right_ray_x * right_ray_x + right_ray_y * right_ray_y);
    
    int fov_length = 15; // Length of FOV visualization lines
    
    // Calculate end points for FOV lines
    int left_end_x = player_minimap_x + (int)((left_ray_x / left_length) * fov_length);
    int left_end_y = player_minimap_y + (int)((left_ray_y / left_length) * fov_length);
    int right_end_x = player_minimap_x + (int)((right_ray_x / right_length) * fov_length);
    int right_end_y = player_minimap_y + (int)((right_ray_y / right_length) * fov_length);
    
    // Draw left FOV line
    dx = left_end_x - player_minimap_x;
    dy = left_end_y - player_minimap_y;
    steps = fmax(fabs(dx), fabs(dy));
    
    if (steps > 0) {
        double x_inc = dx / steps;
        double y_inc = dy / steps;
        double x = player_minimap_x;
        double y = player_minimap_y;
        
        for (int i = 0; i <= steps; i++) {
            my_mlx_pixel_put(data, (int)x, (int)y, 0xeeeeee);  // white for left FOV edge
            x += x_inc;
            y += y_inc;
        }
    }
    
    // Draw right FOV line
    dx = right_end_x - player_minimap_x;
    dy = right_end_y - player_minimap_y;
    steps = fmax(fabs(dx), fabs(dy));
    
    if (steps > 0) {
        double x_inc = dx / steps;
        double y_inc = dy / steps;
        double x = player_minimap_x;
        double y = player_minimap_y;
        
        for (int i = 0; i <= steps; i++) {
            my_mlx_pixel_put(data, (int)x, (int)y, 0xeeeeee);  // white for right FOV edge
            x += x_inc;
            y += y_inc;
        }
    }
    
    // Draw FOV fill (optional - light shading between FOV lines)
    // This creates a triangular area showing the player's field of view
    for (int r = 1; r <= fov_length; r++) {
        double left_x = player_minimap_x + ((left_ray_x / left_length) * r);
        double left_y = player_minimap_y + ((left_ray_y / left_length) * r);
        double right_x = player_minimap_x + ((right_ray_x / right_length) * r);
        double right_y = player_minimap_y + ((right_ray_y / right_length) * r);
        
        // Draw a line connecting the two points
        dx = right_x - left_x;
        dy = right_y - left_y;
        steps = fmax(fabs(dx), fabs(dy));
        
        if (steps > 0) {
            double x_inc = dx / steps;
            double y_inc = dy / steps;
            double x = left_x;
            double y = left_y;
            
            for (int i = 0; i <= steps; i++) {
                my_mlx_pixel_put(data, (int)x, (int)y, 0xeeeeee);  // Transparent white for FOV fill
                x += x_inc;
                y += y_inc;
            }
        }
    }
}