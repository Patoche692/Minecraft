#include "Chunks.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Chunks::Chunks(const char* top, const char* bottom, const char* side)
{
	nbVerticesX = 0;
	nbVerticesY = 0;
	nbVerticesZ = 0;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	for (int i = 0; i < 3; i++)
	{
		int width, height, nrChannels;
		glGenTextures(1, &textures[i]);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = NULL;
		if (i == 0)
			data = stbi_load(top, &width, &height, &nrChannels, 0);
		else if (i == 1)
			data = stbi_load(bottom, &width, &height, &nrChannels, 0);
		else
			data = stbi_load(side, &width, &height, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
	}

	chunks = (unsigned char*****)malloc(sizeof(unsigned char****) * NB_CHUNKS);
	visibleChunks = (unsigned char*****)malloc(sizeof(unsigned char****) * NB_CHUNKS);
	columnHeight = (unsigned int****)malloc(sizeof(unsigned int***) * NB_CHUNKS);
	for (int i = 0; i < NB_CHUNKS; i++)
	{
		chunks[i] = (unsigned char****)malloc(sizeof(unsigned char***) * NB_CHUNKS);
		visibleChunks[i] = (unsigned char****)malloc(sizeof(unsigned char***) * NB_CHUNKS);
		columnHeight[i] = (unsigned int***)malloc(sizeof(unsigned int**) * NB_CHUNKS);
		for (int j = 0; j < NB_CHUNKS; j++)
		{
			chunks[i][j]= (unsigned char***)malloc(sizeof(unsigned char**) * CHUNK_WIDTH);
			visibleChunks[i][j] = (unsigned char***)malloc(sizeof(unsigned char**) * CHUNK_WIDTH);
			columnHeight[i][j] = (unsigned int**)malloc(sizeof(unsigned int*) * CHUNK_WIDTH);
			for (int x = 0; x < CHUNK_WIDTH; x++)
			{
				chunks[i][j][x] = (unsigned char**)malloc(sizeof(unsigned char*) * CHUNK_WIDTH);
				visibleChunks[i][j][x] = (unsigned char**)malloc(sizeof(unsigned char*) * CHUNK_WIDTH);
				columnHeight[i][j][x] = (unsigned int*)malloc(sizeof(unsigned int) * CHUNK_WIDTH);
				for (int z = 0; z < CHUNK_WIDTH; z++)
				{
					chunks[i][j][x][z] = (unsigned char*)malloc(sizeof(unsigned char) * CHUNK_HEIGHT);
					visibleChunks[i][j][x][z] = (unsigned char*)malloc(sizeof(unsigned char) * CHUNK_HEIGHT);

					float cx = (i * CHUNK_WIDTH + x) * (float)(NOISE_FREQUENCY - 1) / (float)NOISE_MAP_SIZE;
					float cz = (j * CHUNK_WIDTH + z) * (float)(NOISE_FREQUENCY - 1) / (float)NOISE_MAP_SIZE;
					float noise = abs(perlinNoise.perlin(cx, cz));
					int value = floor(noise /*+ 1) / 2.0f*/ * (float)100);
					columnHeight[i][j][x][z] = value;

					for (int y = 0; y < CHUNK_HEIGHT; y++)
					{
						if (y<=value)
							chunks[i][j][x][z][y] = 1;
						else
							chunks[i][j][x][z][y] = 0;

						visibleChunks[i][j][x][z][y] = chunks[i][j][x][z][y];
					}
				}
			}
		}
	}

	for (int i = 0; i < NB_CHUNKS; i++)
	{
		for (int j = 0; j < NB_CHUNKS; j++)
		{
			for (int x = 0; x < CHUNK_WIDTH; x++)
			{
				for (int z = 0; z < CHUNK_WIDTH; z++)
				{
					float cx = (i * x + x) * (float)(NOISE_FREQUENCY - 1) / (float)NOISE_MAP_SIZE;
					float cz = (j * z + z) * (float)(NOISE_FREQUENCY - 1) / (float)NOISE_MAP_SIZE;
					float noise = abs(perlinNoise.perlin(cx, cz));
					int value = floor(noise * (float)100);
					for (int y = 0; y <= value; y++)
					{
						if (x < CHUNK_WIDTH - 1 && x > 0 && z < CHUNK_WIDTH - 1 && z > 0 && y < CHUNK_HEIGHT - 1 && y > 0) {
							if (chunks[i][j][x - 1][z][y] && chunks[i][j][x + 1][z][y] && chunks[i][j][x][z - 1][y] && chunks[i][j][x][z + 1][y] && chunks[i][j][x][z][y - 1] && chunks[i][j][x][z][y + 1]) {
								visibleChunks[i][j][x][z][y] = 0;
							}
						}
					}
				}
			}
		}
	}
	//for (int i = 0; i < CHUNK_WIDTH; i++)
	//{
	//	chunks[0][0][0][i][0] = 1;
	//	chunks[0][0][0][i][1] = 1;
	//	chunks[0][0][0][i][2] = 1;
	//}
	//chunks[0][0][1][0][1] = 1;
	//visibleChunks[0][0][1][14][2] = 1;
	//columnHeight[0][0][1][14] = 255;
	//visibleChunks[0][0][1][13][2] = 1;
	//columnHeight[0][0][1][13] = 255;
	//chunks[0][0][0][0][3] = 0;
	//visibleChunks[0][0][0][0][3] = 0;
}

Chunks::~Chunks()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	for (int i = 0; i < NB_CHUNKS; i++)
	{
		for (int j = 0; j < NB_CHUNKS; j++)
		{
			for (int x = 0; x < CHUNK_WIDTH; x++)
			{
				for (int z = 0; z < CHUNK_WIDTH; z++)
				{
					free(chunks[i][j][x][z]);
					free(visibleChunks[i][j][x][z]);
				}
				free(chunks[i][j][x]);
				free(visibleChunks[i][j][x]);
			}
			free(chunks[i][j]);
			free(visibleChunks[i][j]);
		}
		free(chunks[i]);
		free(visibleChunks[i]);
	}
	free(chunks);
	free(visibleChunks);
	free(vertices);
	free(indices);
}

void Chunks::setup()
{

	indices = (unsigned int*)malloc(sizeof(unsigned int) * (nbVerticesX + nbVerticesY + nbVerticesZ) * 6 / 4);
	for (int i = 0; i < (nbVerticesX + nbVerticesY + nbVerticesZ)/4; i++)
	{
		indices[i * 6] = 4 * i;
		indices[i * 6 + 1] = 4 * i + 1;
		indices[i * 6 + 2] = 4 * i + 2;
		indices[i * 6 + 3] = 4 * i + 1;
		indices[i * 6 + 4] = 4 * i + 2;
		indices[i * 6 + 5] = 4 * i + 3;
	}

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * (nbVerticesX + nbVerticesY + nbVerticesZ), vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * (nbVerticesX + nbVerticesY + nbVerticesZ) * 6 / 4, indices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	//for (int i = 0; i < nbVertices / 2; i++)
	//{
	//	printf("x = %d : %f : %f : %f : %f : %f\n", i / 4, vertices[5 * i], vertices[5 * i + 1], vertices[5 * i + 2], vertices[5 * i + 3], vertices[5 * i + 4]);
	//}
}

void Chunks::bindFaces()
{
	nbVerticesX = 0;
	nbVerticesY = 0;
	nbVerticesZ = 0;
	vertices = (float*)realloc(vertices, sizeof(float) * (nbVerticesX + nbVerticesY + nbVerticesZ));
	for (int i = 0; i < NB_CHUNKS; i++)
	{
		for (int j = 0; j < NB_CHUNKS; j++)
		{
			for (int x = 0; x < CHUNK_WIDTH; x++)
			{
				int firstBloc = visibleChunks[i][j][x][0][0], meshWidth = 0, blocType = 1, nzMin = 0;
				for (int z = 0; z < CHUNK_WIDTH; z++)
				{
					meshWidth++;
					if (visibleChunks[i][j][x][z][0] && meshWidth == CHUNK_WIDTH) {
						if (firstBloc != 0) {
							addVertice(CHUNK_WIDTH * i + x, 0.0f, CHUNK_WIDTH * j, 0.0f, 0.0f, X_AXIS);
							addVertice(CHUNK_WIDTH * i + x, 0.0f, CHUNK_WIDTH * (j + 1), meshWidth, 0.0f, X_AXIS);
						}
						recursiveMergingX(i, j, x, 1, 0, CHUNK_WIDTH - 1, 1, firstBloc);
					}
					else if (visibleChunks[i][j][x][z][0] != firstBloc)
					{
						if (firstBloc != 0) {
							addVertice(CHUNK_WIDTH * i + x, 0.0f, CHUNK_WIDTH * j + nzMin, 0.0f, 0.0f, X_AXIS);
							addVertice(CHUNK_WIDTH * i + x, 0.0f, CHUNK_WIDTH * j + z, meshWidth - 1, 0.0f, X_AXIS);
						}
						recursiveMergingX(i, j, x, 1, nzMin, z - 1, 1, firstBloc);
						
						firstBloc = visibleChunks[i][j][x][z][0];
						meshWidth = 0;
						nzMin = z;
						z--;
					}
					else if ((z == CHUNK_WIDTH - 1 && visibleChunks[i][j][x][z][0] == firstBloc)) {
						if (firstBloc != 0) {
							addVertice(CHUNK_WIDTH * i + x, 0.0f, CHUNK_WIDTH * j + nzMin, 0.0f, 0.0f, X_AXIS);
							addVertice(CHUNK_WIDTH * i + x, 0.0f, CHUNK_WIDTH * j + (z + 1), meshWidth, 0.0f, X_AXIS);
						}
						recursiveMergingX(i, j, x, 1, nzMin, z, 1, firstBloc);
					}
				}
			}
		}
	}
	for (int i = 0; i < NB_CHUNKS; i++)
	{
		for (int j = 0; j < NB_CHUNKS; j++)
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				int firstBloc = visibleChunks[i][j][0][0][y], meshWidth = 0, blocType = 1, nxMin = 0;
				for (int x = 0; x < CHUNK_WIDTH; x++)
				{
					meshWidth++;
					if (visibleChunks[i][j][x][0][y] && meshWidth == CHUNK_WIDTH) {
						if (firstBloc != 0) {
							addVertice(CHUNK_WIDTH * i, y, CHUNK_WIDTH * j, 0.0f, 0.0f, Y_AXIS);
							addVertice(CHUNK_WIDTH * (i + 1), y, CHUNK_WIDTH * j, meshWidth, 0.0f, Y_AXIS);
						}
						recursiveMergingY(i, j, y, 1, 0, CHUNK_WIDTH - 1, 1, firstBloc);
					}
					else if (visibleChunks[i][j][x][0][y] != firstBloc)
					{
						if (firstBloc != 0) {
							addVertice(CHUNK_WIDTH * i + nxMin, y, CHUNK_WIDTH * j, 0.0f, 0.0f, Y_AXIS);
							addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j, meshWidth - 1, 0.0f, Y_AXIS);
						}
						recursiveMergingY(i, j, y, 1, nxMin, x - 1, 1, firstBloc);

						firstBloc = visibleChunks[i][j][x][0][y];
						meshWidth = 0;
						nxMin = x;
						x--;
					}
					else if ((x == CHUNK_WIDTH - 1 && visibleChunks[i][j][x][0][y] == firstBloc)) {
						if (firstBloc != 0) {
							addVertice(CHUNK_WIDTH * i + nxMin, y, CHUNK_WIDTH * j, 0.0f, 0.0f, Y_AXIS);
							addVertice(CHUNK_WIDTH * i + x + 1, y, CHUNK_WIDTH * j, meshWidth, 0.0f, Y_AXIS);
						}
						recursiveMergingY(i, j, y, 1, nxMin, x, 1, firstBloc);
					}
				}
			}
		}
	}

	printf("\n%d\n", nbVerticesY);
	int LnbVerticesY = nbVerticesY;
	for (int i = 0; i < LnbVerticesY + 10; i++)
	{
		//printf("%d\n", i);
		//printf("%f\n", vertices[(nbVerticesX - 1) * 5 + i * 5]);
		int x = vertices[(nbVerticesX) * 5 + i * 5];
		int y = vertices[(nbVerticesX) * 5 + i * 5 + 1];
		int z = vertices[(nbVerticesX) * 5 + i * 5 + 2];
		int w = vertices[(nbVerticesX) * 5 + i * 5 + 3];
		int h = vertices[(nbVerticesX) * 5 + i * 5 + 4];
		addVertice(x, y + 1, z, w, h, Y_AXIS);
	}
}

bool Chunks::visible(int i, int j, int x, int z, int y)
{
	if (!x) {
		return true;
	}
	if (x == 1)
		printf("oui : %d\n", chunks[i][j][x - 1][z][y]);
	if (chunks[i][j][x - 1][z][y])
		return false;
	return true;
}

void Chunks::recursiveMergingX(int i, int j, int x, int y, int zMin, int zMax, int meshHeight, unsigned int blocType)
{
	int meshWidth = 0, nzMin = zMin;
	int firstBloc = visibleChunks[i][j][x][zMin][y];
	bool endLastMesh = false;
	if (zMin == zMax && y > columnHeight[i][j][x][zMin] + 1) {
		return;
	}
	for (int z = zMin; z <= zMax; z++)
	{
		meshWidth++;
		if (firstBloc == blocType && meshWidth - 1 == zMax - zMin && visibleChunks[i][j][x][z][y] == firstBloc) {
			if(y < CHUNK_HEIGHT - 1)
				recursiveMergingX(i, j, x, y + 1, zMin, zMax, meshHeight + 1, blocType);
			else if (firstBloc != 0) {
				addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + zMin, 0.0f, meshHeight + 1, X_AXIS);
				addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + (zMax + 1), meshWidth, meshHeight + 1, X_AXIS);
			}
		}
		else if (visibleChunks[i][j][x][z][y] != firstBloc)
		{
			if (blocType != 0 && !endLastMesh) {
				addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + zMin, 0.0f, meshHeight, X_AXIS);			// Fin du mesh précédent
				addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (zMax + 1), zMax - zMin + 1, meshHeight, X_AXIS);
				endLastMesh = true;
			}
				if (firstBloc != 0) {
					addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + nzMin, 0.0f, 0.0f, X_AXIS);
					addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (z), meshWidth - 1, 0.0f, X_AXIS);
				}
				if (y < CHUNK_HEIGHT - 1) 
					recursiveMergingX(i, j, x, y + 1, nzMin, z - 1, 1, firstBloc);
				else if(firstBloc != 0) {
					addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + nzMin, 0.0f, 1.0f, X_AXIS);
					addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + (z), meshWidth - 1, 1.0f, X_AXIS);
				}
			firstBloc = visibleChunks[i][j][x][z][y];
			meshWidth = 0;
			nzMin = z;
			z--;
		}
		else if ((z == zMax && visibleChunks[i][j][x][z][y] == firstBloc)) {
			if (blocType != 0 && !endLastMesh) {
				addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + zMin, 0.0f, meshHeight, X_AXIS);			// Fin du mesh précédent
				addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (zMax + 1), zMax - zMin + 1, meshHeight, X_AXIS);
				endLastMesh = true;
			}
			if (firstBloc != 0) {
				addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + nzMin, 0.0f, 0.0f, X_AXIS);
				addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (z + 1), meshWidth , 0.0f, X_AXIS);
			}
			if(y < CHUNK_HEIGHT - 1)
				recursiveMergingX(i, j, x, y + 1, nzMin, z, 1, firstBloc);
			else if (firstBloc != 0) {
				addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + nzMin, 0.0f, 1.0f, X_AXIS);			// Fin du nouveau mesh
				addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + (z + 1), meshWidth, 1.0f, X_AXIS);
			}
		}
	}
}

void Chunks::recursiveMergingY(int i, int j, int y, int z, int xMin, int xMax, int meshHeight, unsigned int blocType)
{
	int meshWidth = 0, nxMin = xMin;
	int firstBloc = visibleChunks[i][j][xMin][z][y];
	bool endLastMesh = false;
	for (int x = xMin; x <= xMax; x++)
	{
		meshWidth++;
		if (firstBloc == blocType && meshWidth - 1 == xMax - xMin && visibleChunks[i][j][x][z][y] == firstBloc) {
			if (z < CHUNK_WIDTH - 1)
				recursiveMergingY(i, j, y, z + 1, xMin, xMax, meshHeight + 1, blocType);
			else if (firstBloc != 0) {
				addVertice(CHUNK_WIDTH * i + xMin, y, CHUNK_WIDTH * j + z + 1, 0.0f, meshHeight + 1, Y_AXIS);
				addVertice(CHUNK_WIDTH * i + xMax + 1, y, CHUNK_WIDTH * j + z + 1, meshWidth, meshHeight + 1, Y_AXIS);
			}
		}
		else if (visibleChunks[i][j][x][z][y] != firstBloc)
		{
			if (blocType != 0 && !endLastMesh) {
				addVertice(CHUNK_WIDTH * i + xMin, y, CHUNK_WIDTH * j + z, 0.0f, meshHeight, Y_AXIS);			// Fin du mesh précédent
				addVertice(CHUNK_WIDTH * i + xMax + 1, y, CHUNK_WIDTH * j + z, xMax - xMin + 1, meshHeight, Y_AXIS);
				endLastMesh = true;
			}
			if (firstBloc != 0) {
				addVertice(CHUNK_WIDTH * i + nxMin, y, CHUNK_WIDTH * j + z, 0.0f, 0.0f, Y_AXIS);
				addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + z, meshWidth - 1, 0.0f, Y_AXIS);
			}
			if (z < CHUNK_WIDTH - 1)
				recursiveMergingY(i, j, y, z + 1, nxMin, x - 1, 1, firstBloc);
			else if (firstBloc != 0) {
				addVertice(CHUNK_WIDTH * i + nxMin, y, CHUNK_WIDTH * j + z + 1, 0.0f, 1.0f, Y_AXIS);
				addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + z + 1, meshWidth - 1, 1.0f, Y_AXIS);
			}
			firstBloc = visibleChunks[i][j][x][z][y];
			meshWidth = 0;
			nxMin = x;
			x--;
		}
		else if ((x == xMax && visibleChunks[i][j][x][z][y] == firstBloc)) {
			if (blocType != 0 && !endLastMesh) {
				addVertice(CHUNK_WIDTH * i + xMin, y, CHUNK_WIDTH * j + z, 0.0f, meshHeight, Y_AXIS);			// Fin du mesh précédent
				addVertice(CHUNK_WIDTH * i + xMax + 1, y, CHUNK_WIDTH * j + z, xMax - xMin + 1, meshHeight, Y_AXIS);
				endLastMesh = true;
			}
			if (firstBloc != 0) {
				addVertice(CHUNK_WIDTH * i + nxMin, y, CHUNK_WIDTH * j + z, 0.0f, 0.0f, Y_AXIS);
				addVertice(CHUNK_WIDTH * i + x + 1, y, CHUNK_WIDTH * j + z, meshWidth, 0.0f, Y_AXIS);
			}
			if (z < CHUNK_WIDTH - 1)
				recursiveMergingY(i, j, y, z + 1, nxMin, x, 1, firstBloc);
			else if (firstBloc != 0) {
				addVertice(CHUNK_WIDTH * i + nxMin, y, CHUNK_WIDTH * j + z + 1, 0.0f, 1.0f, Y_AXIS);			// Fin du nouveau mesh
				addVertice(CHUNK_WIDTH * i + x + 1, y, CHUNK_WIDTH * j + z + 1, meshWidth, 1.0f, Y_AXIS);
			}
		}
	}
}


void Chunks::addVertice(int x, int y, int z, int s, int t, int axis)
{
	switch (axis)
	{
	case X_AXIS:
		nbVerticesX++;
		vertices = (float*)realloc(vertices, sizeof(float) * 5 * (nbVerticesX + nbVerticesY + nbVerticesZ));
		vertices[5 * (nbVerticesX - 1)] = x;
		vertices[5 * (nbVerticesX - 1) + 1] = y;
		vertices[5 * (nbVerticesX - 1) + 2] = z;
		vertices[5 * (nbVerticesX - 1) + 3] = s;
		vertices[5 * (nbVerticesX - 1) + 4] = t;
		break;
	case Y_AXIS:
		nbVerticesY++;
		vertices = (float*)realloc(vertices, sizeof(float) * 5 * (nbVerticesX + nbVerticesY + nbVerticesZ));
		vertices[5 * (nbVerticesX - 1 + nbVerticesY )] = x;
		vertices[5 * (nbVerticesX - 1 + nbVerticesY ) + 1] = y;
		vertices[5 * (nbVerticesX - 1 + nbVerticesY ) + 2] = z;
		vertices[5 * (nbVerticesX - 1 + nbVerticesY ) + 3] = s;
		vertices[5 * (nbVerticesX - 1 + nbVerticesY ) + 4] = t;
		break;
	default:
		break;
	}

}


void Chunks::draw()
{
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, textures[SIDE]);
	glDrawElements(GL_TRIANGLES, (nbVerticesX) * 6 / 4, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, textures[BOTTOM]);
	glDrawElements(GL_TRIANGLES, (nbVerticesY / 2 + nbVerticesX) * 6 / 4, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, textures[TOP]);
	glDrawElements(GL_TRIANGLES, (nbVerticesY + nbVerticesX) * 6 / 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

//void Chunks::recursiveMerging(int i, int j, int x, int y, int zMin, int zMax, int meshHeight, unsigned int blocType)
//{
//	int meshWidth = 0, nMeshHeight = meshHeight, nzMin = zMin;
//	int firstBloc = chunks[i][j][x][zMin][y];
//	bool endLastMesh = false;
//	if (firstBloc != blocType) {
//		addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + zMin, 0.0f, meshHeight);			// Fin du mesh précédent
//		addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (zMax + 1), zMax - zMin + 1, meshHeight);
//		endLastMesh = true;
//	}
//	for (int z = zMin; z <= zMax; z++)
//	{
//		meshWidth++;
//		if (chunks[i][j][x][z][y] != firstBloc) {
//			if (meshWidth - 1 != zMax - zMin) {
//
//				if (!x && y == 2 && !zMax)
//					printf("%d: %d : %d\n", zMin, y, zMax);
//
//				if (chunks[i][j][x][z][y] != firstBloc) {
//					z--;
//					meshWidth--;
//					nMeshHeight = 0;
//				}
//				else if (z == zMin && chunks[i][j][x][z][y] != blocType) {
//					nMeshHeight = 0;
//				}
//				if (blocType != 0 && !endLastMesh) {
//					addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + zMin, 0.0f, meshHeight);			// Fin du mesh précédent
//					addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (zMax + 1), zMax - zMin + 1, meshHeight);
//					endLastMesh = true;
//				}
//				//else if (z == zMax) {
//
//				//}
//				if (firstBloc != 0 && z >= zMin) {
//					addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + nzMin, 0.0f, 0.0f);
//					addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (z + 1), meshWidth, 0.0f);
//				}
//			}
//			if (y < CHUNK_HEIGHT - 1) {
//				recursiveMerging(i, j, x, y + 1, nzMin, z, nMeshHeight + 1, firstBloc);
//			}
//			//else if(firstBloc != 0) {
//			//	addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + nzMin, 0.0f, 1.0f);
//			//	addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + (z + 1), meshWidth, 1.0f);
//			//}
//			if (z < zMax && chunks[i][j][x][z + 1][y] != firstBloc) {
//				nzMin = z + 1;
//				firstBloc = chunks[i][j][x][nzMin][y];
//				meshWidth = 0;
//			}
//		}
	//	meshWidth++;
	//	if (chunks[i][j][x][z][y] == firstBloc && firstBloc == blocType && meshWidth - 1 == zMax - zMin) {
	//		if (y < CHUNK_HEIGHT - 1)
	//			recursiveMerging(i, j, x, y + 1, zMin, zMax, meshHeight + 1, blocType);
	//		else if (firstBloc != 0) {
	//			addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + zMin, 0.0f, meshHeight + 1);
	//			addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + (zMax + 1), meshWidth, meshHeight + 1);
	//		}
	//	}
	//	else if (chunks[i][j][x][z][y] != firstBloc)
	//	{
	//		if (blocType != 0 && !endLastMesh) {
	//			addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + zMin, 0.0f, meshHeight);			// Fin du mesh précédent
	//			addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (zMax + 1), zMax - zMin + 1, meshHeight);
	//			endLastMesh = true;
	//		}
	//		if (firstBloc != 0) {
	//			addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + nzMin, 0.0f, 0.0f);
	//			addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (z), meshWidth - 1, 0.0f);
	//		}
	//		if (y < CHUNK_HEIGHT - 1)
	//			recursiveMerging(i, j, x, y + 1, nzMin, z - 1, 1, firstBloc);
	//		else if (firstBloc != 0) {
	//			addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + nzMin, 0.0f, 1.0f);
	//			addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + (z), meshWidth - 1, 1.0f);
	//		}
	//		firstBloc = chunks[i][j][x][z][y];
	//		meshWidth = 0;
	//		nzMin = z;
	//		z--;
	//	}
	//	else if ((z == zMax && chunks[i][j][x][z][y] == firstBloc)) {
	//		if (blocType != 0 && !endLastMesh) {
	//			addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + zMin, 0.0f, meshHeight);			// Fin du mesh précédent
	//			addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (zMax + 1), zMax - zMin + 1, meshHeight);
	//			endLastMesh = true;
	//		}
	//		if (firstBloc != 0) {
	//			addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + nzMin, 0.0f, 0.0f);
	//			addVertice(CHUNK_WIDTH * i + x, y, CHUNK_WIDTH * j + (z + 1), meshWidth, 0.0f);
	//		}
	//		if (y < CHUNK_HEIGHT - 1)
	//			recursiveMerging(i, j, x, y + 1, nzMin, z, 1, firstBloc);
	//		else if (firstBloc != 0) {
	//			addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + nzMin, 0.0f, 1.0f);			// Fin du nouveau mesh
	//			addVertice(CHUNK_WIDTH * i + x, y + 1, CHUNK_WIDTH * j + (z + 1), meshWidth, 1.0f);
	//		}
	//	}
//	}
//}