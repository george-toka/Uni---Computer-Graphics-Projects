#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void FrameBufferSizeCallback(GLFWwindow* givenWindow, int givenWidth, int givenHeight)
{
	glViewport(0, 0, givenWidth, givenHeight);
}

void ProcessInput(GLFWwindow* givenWindow)
{
	if (glfwGetKey(givenWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(givenWindow, true);
}


int main()
{
	// Initialization

	if (!glfwInit())
	{
		std::cout << "Failed to initialize glfw!" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(800, 800, "Exercise 2", NULL, NULL);

	if (!window)
	{
		std::cout << "Failed to initialize the window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Set the callback function
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize glew!" << std::endl;
		glfwTerminate();
		return -1;
	}

	const char* glsl_version = "#version 330";
	// Setup Dear ImGui context


	GLfloat  vertices1[] =
	{
		0.40f, 0.0f,  0.0f,
		0.31f, 0.26f, 0.0f,
		0.07f, 0.39f, 0.0f,
	   -0.20f, 0.34f, 0.0f,
	   -0.39f, 0.14f, 0.0f,
	   -0.39f,-0.14f, 0.0f,
	   -0.20f,-0.34f, 0.0f,
		0.07f,-0.39f, 0.0f,
		0.31f,-0.26f, 0.0f
	};


	Shader shaderProgram("res/Shaders_58352/E1_A2_VertexShader.glsl", "res/Shaders_58352/E1_A2_FragmentShader.glsl");

	GLuint VAO1, VBO1;
	GLuint uniID1 = glGetUniformLocation(shaderProgram.ID, "displacement");
	GLuint uniID2 = glGetUniformLocation(shaderProgram.ID, "flags");

	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)3);
	glEnableVertexAttribArray(1);

	//variables to update the uniform-displacement values
	float uniValueX = 0.0f;
	float uniValueY = 0.0f;

	//how much our vertices' value will change on a single key press
	float step = 0.005f;

	//in order to enclose our object in the window, we set (thanks to the fact that it is a normal polygon)
	//an object radius based to our vertices' values. That way our closest-to-the-border edges will prevent any key inputs to change  
	// the object position
	float object_radius = 0.6f;

	glLineWidth(1.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render Using the shader program
		shaderProgram.use();

		//----------------------------------------------------------------------------------------

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && uniValueX < object_radius)
		{
			uniValueX += step;
			glUniform2f(uniID1, uniValueX, uniValueY);
		}

		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && uniValueX > -object_radius)
		{
			uniValueX -= step;
			glUniform2f(uniID1, uniValueX, uniValueY);
		}

		else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && uniValueY < object_radius)
		{
			uniValueY += step;
			glUniform2f(uniID1, uniValueX, uniValueY);
		}

		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && uniValueY > -object_radius)
		{
			uniValueY -= step;
			glUniform2f(uniID1, uniValueX, uniValueY);
		}

		//----------------------------------------------------------------------------------------

		// Draw our polygon
		glDrawArrays(GL_POLYGON, 0, 9);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);

	glfwTerminate();
	return 0;
}
