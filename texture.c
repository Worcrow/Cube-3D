#include "cube3d.h"

unsigned int get_texture_color(t_texture *tex, int y, int x)
{
    char *dst;
    
    dst = tex->addr + (y * tex->line_length + x * (tex->bits_per_pixel / 8));
    return (*(unsigned int*)dst);
}

void load_texture(t_data *data)
{
    char *files_path[] = {
        "./textures/eagle.xpm",
        "./textures/redbrick.xpm",
        "./textures/purplestone.xpm",
        "./textures/greystone.xpm",
        "./textures/bluestone.xpm",
        "./textures/mossy.xpm",
        "./textures/wood.xpm",
        "./textures/colorstone.xpm"
    };
    size_t i;
    int width;
    int height;

    i = 0;
    while (i < 8)
    {
        data->textures[i].img = mlx_xpm_file_to_image(data->mlx, files_path[i], &width, &height);
        if (!data->textures[i].img)
        {
            printf("Error loading texture %s\n", files_path[i]);
            exit(1);
        }
        data->textures[i].addr = mlx_get_data_addr(data->textures[i].img,
            &data->textures[i].bits_per_pixel,
            &data->textures[i].line_length,
            &data->textures[i].endian);
        i++;
    }
}