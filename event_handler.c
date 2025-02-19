#include "cube3d.h"

int key_press(int keycode, t_data *data)
{
    if (keycode == 53)  // ESC key: linux 65307, mac 53
    {
        mlx_destroy_window(data->mlx, data->win);
        exit(0);
    }

    double oldDirX;
    double oldPlaneX;

    // Movement keys (WASD)
    if (keycode == 13 || keycode == 126)  // W or up arrow
    {
        if (!data->worldMap[(int)(data->player.pos_x + data->player.dir_x * MOVE_SPEED)][(int)data->player.pos_y])
            data->player.pos_x += data->player.dir_x * MOVE_SPEED;
        if (!data->worldMap[(int)data->player.pos_x][(int)(data->player.pos_y + data->player.dir_y * MOVE_SPEED)])
            data->player.pos_y += data->player.dir_y * MOVE_SPEED;
    }
    if (keycode == 1 || keycode == 125)   // S or down arrow
    {
        if (!data->worldMap[(int)(data->player.pos_x - data->player.dir_x * MOVE_SPEED)][(int)data->player.pos_y])
            data->player.pos_x -= data->player.dir_x * MOVE_SPEED;
        if (!data->worldMap[(int)data->player.pos_x][(int)(data->player.pos_y - data->player.dir_y * MOVE_SPEED)])
            data->player.pos_y -= data->player.dir_y * MOVE_SPEED;
    }
    if (keycode == 2)   // D - move right
    {
        double moveX = data->player.dir_y * MOVE_SPEED;
        double moveY = -data->player.dir_x * MOVE_SPEED;
        if (!data->worldMap[(int)(data->player.pos_x + moveX)][(int)data->player.pos_y])
            data->player.pos_x += moveX;
        if (!data->worldMap[(int)data->player.pos_x][(int)(data->player.pos_y + moveY)])
            data->player.pos_y += moveY;
    }
    if (keycode == 0)   // A - move left
    {
        double moveX = -data->player.dir_y * MOVE_SPEED;
        double moveY = data->player.dir_x * MOVE_SPEED;
        if (!data->worldMap[(int)(data->player.pos_x + moveX)][(int)data->player.pos_y])
            data->player.pos_x += moveX;
        if (!data->worldMap[(int)data->player.pos_x][(int)(data->player.pos_y + moveY)])
            data->player.pos_y += moveY;
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
