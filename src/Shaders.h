#include <glad/glad.h>
#include <iostream>

class Shaders
{
public:
	Shaders();
	~Shaders();

	unsigned int getShaderProgram();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

private:
	const char* vertexShaderSource = "#version 460 core\n"
									"layout (location = 0) in vec3 aPos;\n"
									"layout(location = 1) in vec2 aTexCoord;\n"
									"uniform mat4 model;\n"
									"uniform mat4 view;\n"
									"uniform mat4 projection;\n"
									"out vec3 ourColor;\n"
									"out vec2 TexCoord;\n"
									"void main()\n"
									"{\n"
									"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
									"	TexCoord = aTexCoord;\n"
									"}\0";
	const char* fragmentShaderSource = "#version 460 core\n"
									"out vec4 FragColor;\n"
									"in vec2 TexCoord;\n"
									"uniform sampler2D ourTexture;\n"
									"void main()\n"
									"{\n"
									"	FragColor = texture(ourTexture, TexCoord);\n"
									"}\n\0";
	unsigned int shaderProgram;
};
