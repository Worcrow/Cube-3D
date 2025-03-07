#include "cube3d.h"

/*
** Advanced collision detection system
** This function checks if a position (new_x, new_y) would result in a collision
** It uses a more precise method by checking multiple points around the player
** Returns 1 if there's a collision, 0 if the position is safe
*/
int check_wall_collision(t_data *data, double new_x, double new_y)
{
    // Define collision radius (player size)
    double radius = COLLISION_BUFFER;
    
    // Check the main position
    if (data->worldMap[(int)new_x][(int)new_y] > 0)
        return (1);
    
    // Check points around the player in a circle
    // This creates a more realistic collision detection for the player's body
    const int num_points = 8; // Number of points to check around the player
    for (int i = 0; i < num_points; i++)
    {
        double angle = 2 * M_PI * i / num_points;
        double check_x = new_x + radius * cos(angle);
        double check_y = new_y + radius * sin(angle);
        
        // Check if this point collides with a wall
        if (data->worldMap[(int)check_x][(int)check_y] > 0)
            return (1);
    }
    
    return (0); // No collision detected
}

/*
** Wall sliding function
** When a player would hit a wall, this function tries to slide along it
** This creates a more natural movement when approaching walls at an angle
*/
void slide_along_wall(t_data *data, double *new_x, double *new_y, double dir_x, double dir_y)
{
    // Try to slide along X axis
    double slide_x = *new_x;
    double slide_y = data->player.pos_y;
    
    if (!check_wall_collision(data, slide_x, slide_y))
    {
        *new_x = slide_x;
        return;
    }
    
    // Try to slide along Y axis
    slide_x = data->player.pos_x;
    slide_y = *new_y;
    
    if (!check_wall_collision(data, slide_x, slide_y))
    {
        *new_y = slide_y;
        return;
    }
    
    // If both direct slides fail, try diagonal slides with reduced movement
    double reduced_move = 0.5; // Reduce movement to half when sliding diagonally
    
    // Try various diagonal slides
    for (int i = 1; i <= 3; i++)
    {
        double factor = reduced_move / i;
        
        // Try different combinations of partial movements
        double test_x = data->player.pos_x + dir_x * factor;
        double test_y = data->player.pos_y + dir_y * factor;
        
        if (!check_wall_collision(data, test_x, test_y))
        {
            *new_x = test_x;
            *new_y = test_y;
            return;
        }
    }
    
    // If all else fails, don't move
    *new_x = data->player.pos_x;
    *new_y = data->player.pos_y;
} 