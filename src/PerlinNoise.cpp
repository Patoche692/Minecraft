#include "PerlinNoise.h"

PerlinNoise::PerlinNoise()
{
	srand((unsigned int)time(0));

	for (int i = 0; i < NOISE_FREQUENCY; i++)
	{
		for (int j = 0; j < NOISE_FREQUENCY; j++)
		{
			float angle = (rand() % 360) * 3.14159265f / 180;

			Gradient[i][j][0] = cos(angle);
			Gradient[i][j][1] = sin(angle);
		}
	}
}

PerlinNoise::~PerlinNoise()
{
}

float PerlinNoise::perlin(float x, float y) {

	int x0 = floor(x);
	int x1 = x0 + 1;
	int y0 = floor(y);
	int y1 = y0 + 1;

	float sx = x - (float)x0;
	float sy = y - (float)y0;

	float n0, n1, ix0, ix1, value;
	n0 = dotGridGradient(x0, y0, x, y);
	n1 = dotGridGradient(x1, y0, x, y);
	ix0 = lerp(n0, n1, sx);
	n0 = dotGridGradient(x0, y1, x, y);
	n1 = dotGridGradient(x1, y1, x, y);
	ix1 = lerp(n0, n1, sx);
	value = lerp(ix0, ix1, sy);

	return value;
}

float PerlinNoise::dotGridGradient(int ix, int iy, float x, float y) {

	float dx = x - (float)ix;
	float dy = y - (float)iy;

	return (dx * Gradient[iy][ix][0] + dy * Gradient[iy][ix][1]);
}

float PerlinNoise::lerp(float a0, float a1, float w) {
	return (1.0f - w) * a0 + w * a1;
}
