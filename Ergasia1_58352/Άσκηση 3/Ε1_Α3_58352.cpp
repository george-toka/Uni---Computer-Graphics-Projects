#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	GLFWwindow* window = glfwCreateWindow(1000, 1000, "Exercise 3", NULL, NULL);

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

	std::cout << glGetString(GL_VERSION) << std::endl;

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

	//Data


	GLfloat vertices[] = {
		//position            //color             
		-0.3f, -0.3f, -0.3f,  1.0f, 0.0f, 0.0,
		0.3f, -0.3f, -0.3f,  1.0f, 0.0f, 0.0,
		0.3f,  0.3f, -0.3f,  1.0f, 0.0f, 0.0,
		0.3f,  0.3f, -0.3f,  1.0f, 0.0f, 0.0,
		-0.3f,  0.3f, -0.3f,  1.0f, 0.0f, 0.0,
		-0.3f, -0.3f, -0.3f,  1.0f, 0.0f, 0.0,

		-0.3f, -0.3f,  0.3f,  0.0f, 1.0f, 0.0,
		0.3f, -0.3f,  0.3f,  0.0f, 1.0f, 0.0,
		0.3f,  0.3f,  0.3f,  0.0f, 1.0f, 0.0,
		0.3f,  0.3f,  0.3f,  0.0f, 1.0f, 0.0,
		-0.3f,  0.3f,  0.3f,  0.0f, 1.0f, 0.0,
		-0.3f, -0.3f,  0.3f,  0.0f, 1.0f, 0.0,

		-0.3f,  0.3f,  0.3f,  0.0f, 0.0f, 1.0f,
		-0.3f,  0.3f, -0.3f,  0.0f, 0.0f, 1.0f,
		-0.3f, -0.3f, -0.3f,  0.0f, 0.0f, 1.0f,
		-0.3f, -0.3f, -0.3f,  0.0f, 0.0f, 1.0f,
		-0.3f, -0.3f,  0.3f,  0.0f, 0.0f, 1.0f,
		-0.3f,  0.3f,  0.3f,  0.0f, 0.0f, 1.0f,

		0.3f,  0.3f,  0.3f,  0.0f, 0.5f, 0.0f,
		0.3f,  0.3f, -0.3f,  0.0f, 0.5f, 0.0f,
		0.3f, -0.3f, -0.3f,  0.0f, 0.5f, 0.0f,
		0.3f, -0.3f, -0.3f,  0.0f, 0.5f, 0.0f,
		0.3f, -0.3f,  0.3f,  0.0f, 0.5f, 0.0f,
		0.3f,  0.3f,  0.3f,  0.0f, 0.5f, 0.0f,

		-0.3f, -0.3f, -0.3f,  0.0f, 0.0f, 0.5f,
		0.3f, -0.3f, -0.3f,  0.0f, 0.0f, 0.5f,
		0.3f, -0.3f,  0.3f,  0.0f, 0.0f, 0.5f,
		0.3f, -0.3f,  0.3f,  0.0f, 0.0f, 0.5f,
		-0.3f, -0.3f,  0.3f,  0.0f, 0.0f, 0.5f,
		-0.3f, -0.3f, -0.3f,  0.0f, 0.0f, 0.5f,

		-0.3f,  0.3f, -0.3f,  0.5f, 0.0f, 0.0f,
		0.3f,  0.3f, -0.3f,  0.5f, 0.0f, 0.0f,
		0.3f,  0.3f,  0.3f,  0.5f, 0.0f, 0.0f,
		0.3f,  0.3f,  0.3f,  0.5f, 0.0f, 0.0f,
		-0.3f,  0.3f,  0.3f,  0.5f, 0.0f, 0.0f,
		-0.3f,  0.3f, -0.3f,  0.5f, 0.0f, 0.0f
	};

	


	// VBO Setup
	unsigned int vertexBufferId;
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader shaderProgram("res/Shaders_58352/E1_A3_VertexShader.glsl", "res/Shaders_58352/E1_A3_FragmentShader.glsl");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	//All Main and Secondary Variables for main loop
	glm::mat4 identity = glm::mat4(1.0f);

	glm::vec3 cubePositions[] =
	{
	  glm::vec3(0.0f,  0.0f,  0.0f),
	  glm::vec3(0.0f,  0.0f,  0.0f),
	  glm::vec3(0.0f,  0.0f,  0.0f)
	};

	float radius12 = 1.5f;
	float radius23 = 2.0f;
	float scale[]  = { (float)(1.0f / 1.3f),(float)(1.0f / 1.5f),(float)(1.0f / 1.2f) };
	float speed[]  = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	bool rotation_state[] = { true,true,true,true,true };

	float prevTime = glfwGetTime();
	float rotation[5] = { 0.0f,0.0f,0.0f,0.0f,0.0f }; //dynamic angle for every rotating way
	float staticAngle[5];							  //array to hold angle value when a rotation_state is stopped to keep the cube in place

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render Using the shader program
		shaderProgram.use();

		//Time steps to be used for rotation method
		float currTime = glfwGetTime();
		if (currTime - prevTime >= 0.001f)
		{
			rotation[0] += 0.01f;  rotation[1] += 0.01f;  rotation[2] += 0.01f;  rotation[3] += 0.01f;  rotation[4] += 0.01f;
			prevTime = currTime;
		}

		//View perspective Setup
		glm::mat4 view;
		view = glm::translate(identity, glm::vec3(0.0f, 0.0f, -8.0f));
		int viewLocation = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

		
		// 3D projection Setup
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(70.0f), 1000.0f / 1000.0f, 0.3f, 100.0f);

		int projectionLocation = glGetUniformLocation(shaderProgram.ID, "projection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

		for (int i = 0; i < 3; i++)
		{
			glm::mat4 model;

			//1st Cube + Rotation 1 
			if (i == 0)
			{
				model = glm::translate(identity, cubePositions[i]);
				
				if (rotation_state[0])
				{
					model = glm::rotate(model, glm::radians(speed[0] * rotation[0] * 10), glm::vec3(0.0f, 1.0f, 0.0f));
					staticAngle[0] = rotation[0];
				}
					
				else
				{
					model = glm::rotate(model, glm::radians(speed[0] * staticAngle[0] * 10), glm::vec3(0.0f, 1.0f, 0.0f));
					rotation[0] = staticAngle[0];
				}

				model = glm::scale(model, glm::vec3(scale[i], scale[i], scale[i]));

				int modelLocation = glGetUniformLocation(shaderProgram.ID, "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			}

//-------------------------------------------------------------------------------------

			//2nd Cube + Rotation 2,4
			else if (i == 1)
			{
				//Rotation 2 is described by the trigonometric formulas below
				//if-else state determines if rotating motion will be done
				//Ignore the weird float factors in sin and cos functions - they are used to calibrated default speed easily
				if (rotation_state[1])
				{
					cubePositions[1] = glm::vec3(radius12 * cos(speed[1] * rotation[1] * 0.3f) * cos(glm::degrees(-45.0f)), //x
												 radius12 * sin(speed[1] * rotation[1] * 0.3f),								//y
												 radius12 * sin(speed[1] * rotation[1] * 0.3f));							//z
					staticAngle[1] = rotation[1];
				}

				else
				{
					cubePositions[1] = glm::vec3(radius12 * cos(speed[1] * staticAngle[1] * 0.3f) * cos(glm::degrees(-45.0f)), //x
												 radius12 * sin(speed[1] * staticAngle[1] * 0.3f),							   //y	
												 radius12 * sin(speed[1] * staticAngle[1] * 0.3f));							   //z	
					rotation[1] = staticAngle[1];
				}

				//Pass to our model matrix our dynamic coordinates for the circular motion
				model = glm::translate(identity, cubePositions[i]);
				
				if(rotation_state[3])
				{
					model = glm::rotate(model, glm::radians(speed[3] * rotation[3] * 10), glm::vec3(1.0f, 0.0f, 0.0f));
					staticAngle[3] = rotation[3];
				}

				else
				{
					model = glm::rotate(model, glm::radians(speed[3] * staticAngle[3] * 10), glm::vec3(1.0f, 0.0f, 0.0f));
					rotation[3] = staticAngle[3];
				}
				
				//scale dem cubos
				model = glm::scale(model, glm::vec3(scale[i], scale[i], scale[i]));

				//pass dem parameters to our uniforms
				int modelLocation = glGetUniformLocation(shaderProgram.ID, "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			}

//-------------------------------------------------------------------------------------

			//3rd Cube + Rotation 2,3,5
			else
			{
				//Rotation 2 is engulfed in cubePositions[1] and Rotation 3 is described by the new trigonometric formulas below
				//if-else state determines if rotating motion will be done
				//Ignore the weird float factors in sin and cos functions - they are used to calibrated default speed easily
				if (rotation_state[2])
				{
					cubePositions[2] = glm::vec3(cubePositions[1].x + radius23 * cos(speed[2] * rotation[2] * 0.3f) * cos(glm::degrees(-45.0f)),
												 cubePositions[1].y + radius23 * sin(speed[2] * rotation[2] * 0.3f),
												 cubePositions[1].z + radius23 * sin(speed[2] * rotation[2] * 0.3f));
					staticAngle[2] = rotation[2];
				}

				else
				{
					cubePositions[2] = glm::vec3(cubePositions[1].x + radius23 * cos(speed[2] * rotation[2] * 0.3f) * cos(glm::degrees(-45.0f)),
												 cubePositions[1].y + radius23 * sin(speed[2] * rotation[2] * 0.3f),
												 cubePositions[1].z + radius23 * sin(speed[2] * rotation[2] * 0.3f));
					rotation[2] = staticAngle[2];
				}

				//Pass to our model matrix our dynamic coordinates for the circular motion
				model = glm::translate(identity, cubePositions[i]);
				
				//Rotation 5 is implemented below
				//If-else does the same job as described before
				if (rotation_state[4])
				{
					model = glm::rotate(model, glm::radians(speed[4] * rotation[4] * 10), glm::vec3(0.0f, 0.0f, 1.0f));
					staticAngle[4] = rotation[4];
				}

				else
				{
					model = glm::rotate(model, glm::radians(speed[4] * staticAngle[4] * 10), glm::vec3(0.0f, 0.0f, 1.0f));
					rotation[4] = staticAngle[4];
				}

				//scale dem cubos
				model = glm::scale(model, glm::vec3(scale[i], scale[i], scale[i]));
				
				//pass dem parameters to our uniforms
				int modelLocation = glGetUniformLocation(shaderProgram.ID, "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			}
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		

		//ImGui Window Calibration
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Tools");

		ImGui::SliderFloat("radius12", &radius12, 1.0f, 1.5f);
		ImGui::SliderFloat("radius23", &radius23, 1.5f, 2.0f);
		
		ImGui::SliderFloat("speed1", &speed[0], 1.0f, 5.0f);    ImGui::SameLine();
		ImGui::Checkbox("rotation1", &rotation_state[0]);

		ImGui::SliderFloat("speed2", &speed[1], 1.0f, 5.0f);	ImGui::SameLine();
		ImGui::Checkbox("rotation2", &rotation_state[1]);

		ImGui::SliderFloat("speed3", &speed[2], 1.0f, 5.0f);	ImGui::SameLine();
		ImGui::Checkbox("rotation3", &rotation_state[2]);

		ImGui::SliderFloat("speed4", &speed[3], 1.0f, 5.0f);	ImGui::SameLine();
		ImGui::Checkbox("rotation4", &rotation_state[3]);

		ImGui::SliderFloat("speed5", &speed[4], 1.0f, 5.0f);	ImGui::SameLine();
		ImGui::Checkbox("rotation5", &rotation_state[4]);


		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	  }

	glfwTerminate();
	return 0;
}
