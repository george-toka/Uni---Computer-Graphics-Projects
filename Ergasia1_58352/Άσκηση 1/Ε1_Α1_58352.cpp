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

bool ToggleMode(GLFWwindow* givenWindow, bool& DefaultModeState)
{

	if (DefaultModeState)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		DefaultModeState = false;
	}

	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		DefaultModeState = true;
	}

	return true;
}


int main()
{
	// Initialization

	if (!glfwInit())
	{
		std::cout << "Failed to initialize glfw!" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(800, 600, "Exercise 1", NULL, NULL);

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
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);


	GLfloat  vertices1[] =
	{
		-0.10f, 0.0f,  0.0f,
		-0.19f, 0.26f, 0.0f,
		-0.43f, 0.39f, 0.0f,
		-0.70f, 0.34f, 0.0f,
		-0.87f, 0.14f, 0.0f,
		-0.87f,-0.14f, 0.0f,
		-0.70f,-0.34f, 0.0f,
		-0.43f,-0.39f, 0.0f,
		-0.19f,-0.26f, 0.0f
	};

	GLfloat  vertices2[] =
	{
		 0.90f, 0.00f, 0.0f,
		 0.75f, 0.31f, 0.0f,
		 0.41f, 0.39f, 0.0f,
		 0.14f, 0.17f, 0.0f,
		 0.14f,-0.17f, 0.0f,
		 0.41f,-0.39f, 0.0f,
		 0.75f,-0.31f, 0.0f
	};

	GLuint VAO1, VBO1, VBO2;

	Shader shaderProgram("res/Shaders_58352/E1_A1_VertexShader.glsl", "res/Shaders_58352/E1_A1_FragmentShader.glsl");

	glLineWidth(1.0f);

	//Enable the opacity effect
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ImVec4 clear_color1 = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 clear_color2 = ImVec4(1.0f, 0.0f, 0.00f, 0.00f);

	bool DefaultModeState = true;
	bool W_KEY_PRESSED = false;
	bool W_KEY_RELEASED = true;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		glClear(GL_COLOR_BUFFER_BIT);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Render Using the shader program
		shaderProgram.use();

		// Draw the first polygon
		glGenVertexArrays(1, &VAO1);
		glGenBuffers(1, &VBO1);

		glBindBuffer(GL_ARRAY_BUFFER, VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);
		glEnableVertexAttribArray(0);

		int uniformLocationColor1 = glGetUniformLocation(shaderProgram.ID, "color");
		glUniform4f(uniformLocationColor1, clear_color1.x, clear_color1.y, clear_color1.z, clear_color1.w);

		glDrawArrays(GL_POLYGON, 0, 9);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// Draw the second polygon
		glGenBuffers(2, &VBO2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);
		glEnableVertexAttribArray(0);

		int uniformLocationColor2 = glGetUniformLocation(shaderProgram.ID, "color");
		glUniform4f(uniformLocationColor2, clear_color2.x, clear_color2.y, clear_color2.z, clear_color2.w);

		glDrawArrays(GL_POLYGON, 0, 7);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);


		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && W_KEY_PRESSED != W_KEY_RELEASED)
			W_KEY_PRESSED = ToggleMode(window, DefaultModeState);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
			W_KEY_PRESSED = false;

		ImGui::Begin("Hello, world!");

		ImGui::SliderFloat("intensity1", &clear_color1.w, 0.0f, 1.0f);
		ImGui::SliderFloat("intensity2", &clear_color2.w, 0.0f, 1.0f);

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &VBO2);

	glfwTerminate();
	return 0;
}
