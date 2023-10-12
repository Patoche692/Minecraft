#pragma once
#include <glad/glad.h>
#include <iostream>
#include "PerlinNoise.h"
#define CHUNK_WIDTH (16)
#define CHUNK_HEIGHT (256)
#define NB_CHUNKS (10)

typedef struct Pos
{
	int x, y, z;
};

enum { TOP, BOTTOM, SIDE };
enum { X_AXIS, Y_AXIS, Z_AXIS};

class Chunks
{
public:
	Chunks(const char* top, const char* bottom, const char* side);
	~Chunks();
	void setup();
	void bindFaces();
	int bindFacesOfChunk(int i, int j);
	bool visible(int i, int j, int x, int z, int y);
	void recursiveMergingX(int i, int j, int x, int y, int zMin, int zMax, int meshHeight, unsigned int blocType);
	void recursiveMergingY(int i, int j, int y, int z, int xMin, int xMax, int meshHeight, unsigned int blocType);
	void addVertice(int x, int y, int z, int s, int t, int axis);
	void draw();
	unsigned char***** chunks, ***** visibleChunks;
	unsigned int**** columnHeight;
private:
	PerlinNoise perlinNoise;
	float* vertices;
	unsigned int textures[3];
	unsigned int VBO, VAO, EBO;
	int nbVerticesX, nbVerticesY, nbVerticesZ;
	unsigned int* indices;
};
