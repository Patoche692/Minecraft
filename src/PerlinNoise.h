#ifndef DEF_PERLINNOISE
#define DEF_PERLINNOISE

#include <random>
#include <time.h>

#define NOISE_FREQUENCY (2)
#define NOISE_MAP_SIZE (160)

class PerlinNoise
{
public:
	PerlinNoise();
	~PerlinNoise();
	float dotGridGradient(int ix, int iy, float x, float y);
	float perlin(float x, float y);
	float lerp(float a0, float a1, float w);

private:
	float Gradient[5][5][2];
};

#endif // !DEF_PERLINNOISE
