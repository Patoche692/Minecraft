#include "Map.h"

Map::Map()
{
	cubeMap.resize(NOISE_MAP_SIZE);

	for (int x = 0; x < NOISE_MAP_SIZE; x++)
	{
		for (int y = 0; y < NOISE_MAP_SIZE; y++)
		{
			float cx = x * (float)(NOISE_FREQUENCY - 1) / (float)NOISE_MAP_SIZE;
			float cy = y * (float)(NOISE_FREQUENCY - 1) / (float)NOISE_MAP_SIZE;

			cubeMap[x].push_back(perlinNoise.perlin(cx, cy));
			if (cubeMap[x][y]<0)
			{
				cubeMap[x][y] *= -1;
			}
		}
	}
}

Map::~Map()
{
}

float Map::getNoiseMap(int x, int y)
{
	return cubeMap[x][y];
}