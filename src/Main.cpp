#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Map.h"
#include "Shaders.h"
#include "Camera.h"
//#include "Cube.h"
#include "Chunks.h"

vector<vector<vector<int>>> cubeMap;
Camera camera(glm::vec3(0.0f, 0.0f, 7.0f));
float deltaTime = 0.0f;	// time between current frame and last frame
float lastX = 1200 / 2.0f;
float lastY = 800 / 2.0f;
bool firstMouse = true;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
	GLFWwindow* window = glfwCreateWindow(1200, 800, "Minecraft", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);/*
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);*/

    Map map;
    Shaders shaders;
	//Cube cube("grass_block_top.jpg", "grass_block_bottom.jpg", "grass_block_side.jpg");
	Chunks chunks("grass_block_top.jpg", "grass_block_bottom.jpg", "grass_block_side.jpg");


	cubeMap.resize(NOISE_MAP_SIZE);
	for (int x = 0; x < NOISE_MAP_SIZE; x++)
	{
		cubeMap[x].resize(NOISE_MAP_SIZE);
		for (int z = 0; z < NOISE_MAP_SIZE; z++)
		{
			int value = floor(map.getNoiseMap(x, z)*(float)NOISE_MAP_SIZE);
			
			for (int y = 0; y < NOISE_MAP_SIZE; y++)
			{
				if (y<=value){
					cubeMap[x][z].push_back(1);
				}
				else {
					cubeMap[x][z].push_back(0);
				}
			}
		}
	}

	float start = glfwGetTime();
	chunks.bindFaces();
	chunks.setup();
	float end = glfwGetTime();
	printf("%f", end - start);

	glUseProgram(shaders.getShaderProgram());

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)1200 / (float)800, 0.1f, 200.0f);
	unsigned int projectionLoc = glGetUniformLocation(shaders.getShaderProgram(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    while (!glfwWindowShouldClose(window))
    {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		//printf("%f\n", 1/deltaTime);
		
		lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 model = glm::mat4(1.0f);

		glm::mat4 view = camera.GetViewMatrix();

		unsigned int viewLoc = glGetUniformLocation(shaders.getShaderProgram(), "view");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

		unsigned int modelLoc = glGetUniformLocation(shaders.getShaderProgram(), "model");

		glm::mat4 identity = glm::mat4(1.0f);
		//cube.useVAO();

		int k = 0;
		//for (int x = 0; x < NOISE_MAP_SIZE; x++)
		//{
		//	for (int z = 0; z < NOISE_MAP_SIZE; z++)
		//	{
		//		int value = floor(map.getNoiseMap(x, z) * (float)NOISE_MAP_SIZE);

				//for (int y = 0; y <= value; y++)
				//{
				//	if (cubeMap[x][z][y])
				//	{
						//model = glm::translate(identity, glm::vec3(x, value, z));

						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

						//cube.draw();
				chunks.draw();
					//}
				//}
		//	}
		//}
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}

