#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

#include "stb_image/stb_image.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

Camera myCamera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;
float currentFrameTime = 0.0f;
float previousFrameTime = 0.0f;

float previousMousePositonX = 400.0f;
float previousMousePositonY = 300.0f;

bool firstMouse = true;

void FrameBufferSizeCallback(GLFWwindow* givenWindow, int givenWidth, int givenHeight)
{
	glViewport(0, 0, givenWidth, givenHeight);
}

void ProcessInput(GLFWwindow* givenWindow)
{

	if (glfwGetKey(givenWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(givenWindow, true);


	if (glfwGetKey(givenWindow, GLFW_KEY_W) == GLFW_PRESS)
		myCamera.ProcessKeyboard(FORWARD, deltaTime);

	if (glfwGetKey(givenWindow, GLFW_KEY_S) == GLFW_PRESS)
		myCamera.ProcessKeyboard(BACKWARD, deltaTime);

	if (glfwGetKey(givenWindow, GLFW_KEY_A) == GLFW_PRESS)
		myCamera.ProcessKeyboard(LEFT, deltaTime);

	if (glfwGetKey(givenWindow, GLFW_KEY_D) == GLFW_PRESS)
		myCamera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(givenWindow, GLFW_KEY_E) == GLFW_PRESS)
		myCamera.ProcessKeyboard(UP, deltaTime);

	if (glfwGetKey(givenWindow, GLFW_KEY_Q) == GLFW_PRESS)
		myCamera.ProcessKeyboard(DOWN, deltaTime);
}

void MousPositionCallback(GLFWwindow* givenWindow, double givenMousePositionX, double givenMousePositionY)
{
	//std::cout << givenMousePositionX << std::endl;
	//std::cout << givenMousePositionY << std::endl;

	if (firstMouse)
	{
		previousMousePositonX = givenMousePositionX;
		previousMousePositonY = givenMousePositionY;

		firstMouse = false;
	}

	float offsetX = givenMousePositionX - previousMousePositonX;
	float offsetY = previousMousePositonY - givenMousePositionY;
	previousMousePositonX = givenMousePositionX;
	previousMousePositonY = givenMousePositionY;

	myCamera.ProcessMouseMovement(offsetX, offsetY);
}

void ScrollCallback(GLFWwindow* givenWindow, double givenScrollOffsetX, double givenScrollOffsetY)
{
	myCamera.ProcessMouseScroll(givenScrollOffsetY);
}

unsigned int GenerateTexture(const char* givenTextureFilePath, GLuint wrappingMode)
{
	unsigned int textureId;
	glGenTextures(1, &textureId);

	int imageWidth, imageHeight, numberOfchannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* imageData;
	imageData = stbi_load(givenTextureFilePath, &imageWidth, &imageHeight, &numberOfchannels, 0);

	int imageFormat = 3;

	if (numberOfchannels == 1)
		imageFormat = GL_RED;
	else if (numberOfchannels == 3)
		imageFormat = GL_RGB;
	else if (numberOfchannels == 4)
		imageFormat = GL_RGBA;

	if (imageData)
	{
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, imageWidth, imageHeight, 0, imageFormat, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
		std::cout << "Could not read the image!!!" << std::endl;

	stbi_image_free(imageData);
	return textureId;
}

unsigned int GenerateMatrixTexture(const char* givenTextureFilePath)
{
	unsigned int textureId;
	glGenTextures(1, &textureId);

	int imageWidth, imageHeight, numberOfchannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* imageData;
	imageData = stbi_load(givenTextureFilePath, &imageWidth, &imageHeight, &numberOfchannels, 0);

	int imageFormat = 3;

	if (numberOfchannels == 1)
		imageFormat = GL_RED;
	else if (numberOfchannels == 3)
		imageFormat = GL_RGB;
	else if (numberOfchannels == 4)
		imageFormat = GL_RGBA;

	if (imageData)
	{
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, imageWidth, imageHeight, 0, imageFormat, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
		std::cout << "Could not read the image!!!" << std::endl;

	stbi_image_free(imageData);
	return textureId;
}

int main()
{

	// Initialization

	if (!glfwInit())
	{
		std::cout << "Failed to initialize glfw!" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Lab5", NULL, NULL);

	if (!window)
	{
		std::cout << "Failed to initialize the window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Set the callback function
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	glfwSetCursorPosCallback(window, MousPositionCallback);

	glfwSetScrollCallback(window, ScrollCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize glew!" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	// Setup Dear ImGui context
	const char* glsl_version = "#version 330";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);


	//Data

	//--Vertex Data
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, //back
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, //front
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, //left
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,	//right
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,	//down
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f,	//up
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
	};

	glm::vec3 cubePositions[] =
	{
	  glm::vec3(-3.0f, 0.0f, 0.0f),
	  glm::vec3(-1.0f, 0.0f, 0.0f),
	  glm::vec3(1.0f, 0.0f, 0.0f),
	  glm::vec3(3.0f, 0.0f, 0.0f)
	};

	//transparent plane to host our matrix texture over our 3rd cube 
	float plane[] = {
		
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,	0.0f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,	0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
	};

	glm::vec3 planePositions[] =
	{
		glm::vec3(1.0f, 0.0f, -0.11f),
		glm::vec3(1.0f, 0.0f,  0.91f),
		glm::vec3(0.895f, 0.0f, 0.0f),
		glm::vec3(1.91f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.105f, 0.0f),
		glm::vec3(1.0f, -0.905f, 0.0f)
	};

	float planeRotationAngle = 90.0f;

	glm::vec3 planeRotationAxis[] =
	{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	};

	glm::vec3 planeScale = glm::vec3(0.8f);

	glm::vec3 lightSourcePosition(0.0f, 0.0f, 5.0f);
	glm::vec3 lightSourceDirection(0.0f, 0.0f, -1.0f); 

	// first buffer (vbo)
	unsigned int vertexBufferId;
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// second buffer (vbo)
	unsigned int vertexBufferId2;

	Shader shaderProgram("res/Shaders_58352/E3_A1_VertexShader1.txt", "res/Shaders_58352/E3_A1_FragmentShader1.txt");
	Shader lightSourceShaderProgram("res/Shaders/LightSourceVertexShader.glsl", "res/Shaders/LightSourceFragmentShader.glsl");

	glm::mat4 identity = glm::mat4(1.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	GLuint wrappingMode = GL_REPEAT;

	unsigned int awesomeFaceTextureId = GenerateTexture("res/Images/awesomeface.png", wrappingMode);
	unsigned int containerTextureId = GenerateTexture("res/Images/container.jpg", wrappingMode);
	unsigned int container2TextureId = GenerateTexture("res/Images/container2.png", wrappingMode);
	unsigned int container2SpecularTextureId = GenerateTexture("res/Images/container2_specular.png", wrappingMode);
	unsigned int matrixTextureId = GenerateMatrixTexture("res/Images/matrix.jpg");

	//For the 1st Cube
	bool SwapSwitch = false;

	//For the 2nd Cube
	float scaleFactor = 1.0f;

	//For the 3rd Cube
	bool animationSwitch = false;

	float animationOffset = 0;

	//For the 4th Cube
	int textureChoice = 3;

	glEnable(GL_BLEND);// you enable blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		float time = glfwGetTime();

		currentFrameTime = time;
		deltaTime = currentFrameTime - previousFrameTime;
		previousFrameTime = currentFrameTime;

		//std::cout << deltaTime << std::endl;

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region Texture_Loading For Object 1,2,3,4 

		// Render Using the shader program
		shaderProgram.use();

		//Materials
		
//---------------------------------------------------------------------1
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, awesomeFaceTextureId);
		shaderProgram.setInt("material1.awesomeFaceTexture", 0);

		shaderProgram.setFloat("material1.shininess", 128.0f * 2.0f);
//---------------------------------------------------------------------2
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, containerTextureId);
		shaderProgram.setInt("material2.containerTexture", 1);

		shaderProgram.setInt("material2.awesomeFaceTexture", 0);
//---------------------------------------------------------------------3
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, container2TextureId);
		shaderProgram.setInt("material3.container2Texture", 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, container2SpecularTextureId);
		shaderProgram.setInt("material3.container2SpecularTexture", 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, matrixTextureId);

		shaderProgram.setInt("material3.matrixTexture", 4);

		shaderProgram.setFloat("material3.shininess", 128.0f * 2.0f);

#pragma endregion 

		shaderProgram.setVec3("cameraPosition", myCamera.Position);

		// Light Source
		shaderProgram.setVec3("lightSource.direction", lightSourceDirection); 
		shaderProgram.setVec3("lightSource.ambientColor", glm::vec3(1.0f, 1.0f, 1.0f));
		shaderProgram.setVec3("lightSource.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
		shaderProgram.setVec3("lightSource.specularColor", glm::vec3(1.0f, 1.0f, 1.0f));


		glm::mat4 view;
		view = myCamera.GetViewMatrix(); // now we get the view matrix form the camrera class -------------
		shaderProgram.setMat4("view", view);

		glm::mat4 projection;

		// 3D
		projection = glm::perspective(glm::radians(myCamera.Zoom), 800.0f / 600.0f, 0.3f, 100.0f); // now we get the zoom form the camera class ---------------------

		shaderProgram.setMat4("projection", projection);

		//5th
		for (int i = 0; i < 4; i++)
		{
			shaderProgram.setInt("VertMaterialIndex", i);
			shaderProgram.setInt("FragMaterialIndex", i);

			glm::mat4 model;

			model = glm::translate(identity, cubePositions[i]);

			shaderProgram.setMat4("model", model);

			glm::mat3 normalTransformation = glm::transpose(glm::inverse(glm::mat3(model)));
			shaderProgram.setMat3("normalTransformation", normalTransformation);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		shaderProgram.setInt("VertMaterialIndex", 4);
		shaderProgram.setInt("FragMaterialIndex", 4);

#pragma region ImGui Interface

		//ImGui Frame Initialisation
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Tools Window");

		//1st Cube - Swap the texture Coords

		shaderProgram.setBool("SwapSwitch", SwapSwitch);

		if (ImGui::Button("Swap Me!"))
		{
			SwapSwitch = !SwapSwitch;
			shaderProgram.setBool("SwapSwitch", SwapSwitch);
		}

		//2nd Cube - Add/Substract number of awesome-faces rows on container
		
		shaderProgram.setFloat("scaleFactor", scaleFactor);

		if (ImGui::Button("plus"))
		{
			scaleFactor = scaleFactor + 1;
			shaderProgram.setFloat("scaleFactor", scaleFactor);
		}

		ImGui::SameLine();

		if (ImGui::Button("minus"))
		{
			if (scaleFactor > 1)
			{
				scaleFactor = scaleFactor - 1;
				shaderProgram.setFloat("scaleFactor", scaleFactor);
			}
		}

		if (ImGui::Button("change wrapping"))
		{
			glDeleteTextures(1, &awesomeFaceTextureId);

			if (wrappingMode == GL_REPEAT)
				wrappingMode = GL_MIRRORED_REPEAT;
			else
				wrappingMode = GL_REPEAT;

			awesomeFaceTextureId = GenerateTexture("res/Images/awesomeface.png", wrappingMode);
		}

		//4th Cube Texture Combo

		static const char* textures[] = { "awesome face", "container", "container 2 + specular", "matrix" };
		shaderProgram.setInt("textureChoice", textureChoice);

		ImGui::Combo("texture", &textureChoice, textures, IM_ARRAYSIZE(textures));

		//3rd Cube Animation
		glGenBuffers(1, &vertexBufferId2);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (const void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (const void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (const void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (const void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (const void*)(10 * sizeof(float)));
		glEnableVertexAttribArray(4);


		for (int i = 0; i < 6; i++)
		{
			glm::mat4 model;

			model = glm::translate(identity, planePositions[i]);

			if (i > 1)
				model = glm::rotate(model, glm::radians(planeRotationAngle), planeRotationAxis[i]);

			model = glm::scale(model, planeScale);

			shaderProgram.setMat4("model", model);

			glm::mat3 normalTransformation = glm::transpose(glm::inverse(glm::mat3(model)));
			shaderProgram.setMat3("normalTransformation", normalTransformation);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		ImGui::Checkbox("animation", &animationSwitch);

		shaderProgram.setFloat("deltaTime", animationOffset);


		if (animationSwitch)
			animationOffset = animationOffset + deltaTime;

		ImGui::End();

		//Render Dear ImGui Loop
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


#pragma endregion

		glGenBuffers(1, &vertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (const void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		// Render Using the light source shader program
		lightSourceShaderProgram.use();

		lightSourceShaderProgram.setMat4("view", view);
		lightSourceShaderProgram.setMat4("projection", projection);

		glm::mat4 model = glm::translate(identity, lightSourcePosition);
		lightSourceShaderProgram.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
