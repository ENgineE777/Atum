
#include "Texture.h"

void Texture::NextMip(int& width, int& height, int block_size)
{
	width = (width > block_size) ? (width >> 1) : block_size;
	height = (height > block_size) ? (height >> 1) : block_size;
}

int Texture::GetLevels(int width, int height, int block_size)
{
	int w = width;
	int h = height;

	int levels = 0;

	while (w != 1 && h != 1)
	{
		NextMip(w, h, block_size);
		levels++;
	}

	return levels;
}