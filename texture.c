#include "cube3d.h"

void initialize_texture(t_data *data)
{
      
    size_t i;

    data->buffer = malloc(sizeof(unsigned int *) * WINDOW_HEIGHT);
    data->textures = malloc(sizeof(unsigned int *) * 8);
    i = 0;
    if (!data->buffer)
    {
        printf("Error: Failed to allocate memory for buffer\n");
        exit(1);
    }
    if (!data->textures)
    {
        printf("Error: Failed to allocate memory for texture\n");
        exit(1);
    }
    while (i < WINDOW_HEIGHT)
    {
        data->buffer[i] = malloc(sizeof(unsigned int) * WINDOW_WIDTH);
        if (!data->buffer[i])
        {
            printf("Error: Failed to allocate memory for texture buffer\n");
            exit(1);
        }
        ++i;
    }
    i = 0;
    while (i < 8)
    {
        data->textures[i] = malloc(sizeof(unsigned int) * TEXTURE_WIDTH * TEXTURE_HEIGHT);
        if (!data->textures[i])
        {
            printf("Error: Failed to allocate memory for texture\n");
            exit(1);
        }
        ++i;
    }
    for(int x = 0; x < TEXTURE_WIDTH; x++)
    {
        for(int y = 0; y < TEXTURE_HEIGHT; y++)
        {
            int xorcolor = (x * 256 / TEXTURE_WIDTH) ^ (y * 256 / TEXTURE_HEIGHT);
    //int xcolor = x * 256 / texWidth;
            int ycolor = y * 256 / TEXTURE_HEIGHT;
            int xycolor = y * 128 / TEXTURE_HEIGHT + x * 128 / TEXTURE_WIDTH;
            data->textures[0][TEXTURE_WIDTH * y + x] = 65536 * 254 * (x != y && x != TEXTURE_WIDTH - y); //flat red texture with black cross
            data->textures[1][TEXTURE_WIDTH * y + x] = xycolor + 256 * xycolor + 65536 * xycolor; //sloped greyscale
            data->textures[2][TEXTURE_WIDTH * y + x] = 256 * xycolor + 65536 * xycolor; //sloped yellow gradient
            data->textures[3][TEXTURE_WIDTH * y + x] = xorcolor + 256 * xorcolor + 65536 * xorcolor; //xor greyscale
            data->textures[4][TEXTURE_WIDTH * y + x] = 256 * xorcolor; //xor green
            data->textures[5][TEXTURE_WIDTH * y + x] = 65536 * 192 * (x % 16 && y % 16); //red bricks
            data->textures[6][TEXTURE_WIDTH * y + x] = 65536 * ycolor; //red gradient
            data->textures[7][TEXTURE_WIDTH * y + x] = 128 + 256 * 128 + 65536 * 128; //flat grey texture
        }
    }
}