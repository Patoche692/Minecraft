#ifndef DEF_MAP
#define DEF_MAP

#include "PerlinNoise.h"
#include <vector>

#define NOISE_MAP_SIZE (800)

using namespace std;

class Map
{
public:
	Map();
	~Map();
	float getNoiseMap(int x, int y);

private:
	vector<vector<float>> cubeMap;
	PerlinNoise perlinNoise;
};

#endif // !DEF_MAP
