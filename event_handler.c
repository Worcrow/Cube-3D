#include "cube3d.h"

// Function prototype for slide_along_wall (defined in collision.c)
void slide_along_wall(t_data *data, double *new_x, double *new_y, double dir_x, double dir_y);

// Handle mouse entering the window
int mouse_enter(t_data *data)
{
    // Reset the previous mouse position when entering the window
    // This will be handled in mouse_move
    return (0);
}

// Handle mouse leaving the window
int mouse_leave(t_data *data)
{
    // Nothing special to do when mouse leaves
    return (0);
}

// Mouse movement handler for player rotation
int mouse_move(int x, int y, t_data *data)
{
    // Store the previous mouse position
    static int prev_x = -1;
    
    // Check if mouse is within window boundaries
    if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
    {
        // Mouse is outside the window, don't rotate but update prev_x
        // This prevents a sudden jump when the mouse re-enters the window
        prev_x = -1;
        return (0);
    }
    
    // Initialize prev_x on first call or when re-entering the window
    if (prev_x == -1)
    {
        prev_x = x;
        return (0);
    }
    
    // Calculate mouse movement delta
    int delta_x = x - prev_x;
    
    // Only rotate if there's significant movement
    if (abs(delta_x) > 0)
    {
        // Calculate rotation angle based on mouse movement
        // Adjust sensitivity as needed
        double rotation_angle = delta_x * 0.003;
        
        // Rotate direction vector
        double oldDirX = data->player.dir_x;
        data->player.dir_x = data->player.dir_x * cos(-rotation_angle) - data->player.dir_y * sin(-rotation_angle);
        data->player.dir_y = oldDirX * sin(-rotation_angle) + data->player.dir_y * cos(-rotation_angle);
        
        // Rotate camera plane
        double oldPlaneX = data->player.plane_x;
        data->player.plane_x = data->player.plane_x * cos(-rotation_angle) - data->player.plane_y * sin(-rotation_angle);
        data->player.plane_y = oldPlaneX * sin(-rotation_angle) + data->player.plane_y * cos(-rotation_angle);
        
        // Update the frame
        mlx_destroy_image(data->mlx, data->img);
        data->img = mlx_new_image(data->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
        data->addr = mlx_get_data_addr(data->img, &data->bits_per_pixel, &data->line_length, &data->endian);
        render_frame(data);
    }
    
    // Update previous mouse position
    prev_x = x;
    
    return (0);
}

int key_press(int keycode, t_data *data)
{
    if (keycode == 53)  // ESC key: linux 65307, mac 53
    {
        mlx_destroy_window(data->mlx, data->win);
        exit(0);
    }

    double oldDirX;
    double oldPlaneX;
    double moveSpeed = MOVE_SPEED;

    // Movement keys (WASD)
    if (keycode == 13 || keycode == 126)  // W or up arrow
    {
        // Calculate new position
        double newPosX = data->player.pos_x + data->player.dir_x * moveSpeed;
        double newPosY = data->player.pos_y + data->player.dir_y * moveSpeed;
        
        // Check for collision and handle wall sliding if needed
        if (check_wall_collision(data, newPosX, newPosY))
            slide_along_wall(data, &newPosX, &newPosY, data->player.dir_x, data->player.dir_y);
        else
        {
            data->player.pos_x = newPosX;
            data->player.pos_y = newPosY;
        }
    }
    if (keycode == 1 || keycode == 125)   // S or down arrow
    {
        // Calculate new position
        double newPosX = data->player.pos_x - data->player.dir_x * moveSpeed;
        double newPosY = data->player.pos_y - data->player.dir_y * moveSpeed;
        
        // Check for collision and handle wall sliding if needed
        if (check_wall_collision(data, newPosX, newPosY))
            slide_along_wall(data, &newPosX, &newPosY, -data->player.dir_x, -data->player.dir_y);
        else
        {
            data->player.pos_x = newPosX;
            data->player.pos_y = newPosY;
        }
    }
    if (keycode == 2)   // D - move right
    {
        // Calculate perpendicular movement vector (right)
        double moveX = data->player.dir_y * moveSpeed;
        double moveY = -data->player.dir_x * moveSpeed;
        
        // Calculate new position
        double newPosX = data->player.pos_x + moveX;
        double newPosY = data->player.pos_y + moveY;
        
        // Check for collision and handle wall sliding if needed
        if (check_wall_collision(data, newPosX, newPosY))
            slide_along_wall(data, &newPosX, &newPosY, moveX, moveY);
        else
        {
            data->player.pos_x = newPosX;
            data->player.pos_y = newPosY;
        }
    }
    if (keycode == 0)   // A - move left
    {
        // Calculate perpendicular movement vector (left)
        double moveX = -data->player.dir_y * moveSpeed;
        double moveY = data->player.dir_x * moveSpeed;
        
        // Calculate new position
        double newPosX = data->player.pos_x + moveX;
        double newPosY = data->player.pos_y + moveY;
        
        // Check for collision and handle wall sliding if needed
        if (check_wall_collision(data, newPosX, newPosY))
            slide_along_wall(data, &newPosX, &newPosY, moveX, moveY);
        else
        {
            data->player.pos_x = newPosX;
            data->player.pos_y = newPosY;
        }
    }

    // Rotation keys (left and right arrows)
    if (keycode == 123)  // Left arrow - rotate left
    {
        oldDirX = data->player.dir_x;
        data->player.dir_x = data->player.dir_x * cos(ROTATION_SPEED) - data->player.dir_y * sin(ROTATION_SPEED);
        data->player.dir_y = oldDirX * sin(ROTATION_SPEED) + data->player.dir_y * cos(ROTATION_SPEED);
        oldPlaneX = data->player.plane_x;
        data->player.plane_x = data->player.plane_x * cos(ROTATION_SPEED) - data->player.plane_y * sin(ROTATION_SPEED);
        data->player.plane_y = oldPlaneX * sin(ROTATION_SPEED) + data->player.plane_y * cos(ROTATION_SPEED);
    }
    if (keycode == 124)  // Right arrow - rotate right
    {
        oldDirX = data->player.dir_x;
        data->player.dir_x = data->player.dir_x * cos(-ROTATION_SPEED) - data->player.dir_y * sin(-ROTATION_SPEED);
        data->player.dir_y = oldDirX * sin(-ROTATION_SPEED) + data->player.dir_y * cos(-ROTATION_SPEED);
        oldPlaneX = data->player.plane_x;
        data->player.plane_x = data->player.plane_x * cos(-ROTATION_SPEED) - data->player.plane_y * sin(-ROTATION_SPEED);
        data->player.plane_y = oldPlaneX * sin(-ROTATION_SPEED) + data->player.plane_y * cos(-ROTATION_SPEED);
    }

    mlx_destroy_image(data->mlx, data->img);
    data->img = mlx_new_image(data->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
    data->addr = mlx_get_data_addr(data->img, &data->bits_per_pixel, &data->line_length, &data->endian);
    render_frame(data);
    return (0);
}
