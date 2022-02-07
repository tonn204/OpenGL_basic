#include <iostream>
#include <vector>
#include <typeinfo>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void calculateVertices(std::vector<glm::vec3> & vertices, glm::vec3 a, glm::vec3 b, glm::vec3 c);
void calculateFinalVertices(int n, int &verts_number, std::vector<float>& vertices, glm::vec3 a, glm::vec3 b, glm::vec3 c);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Window!", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);

	std::vector<float> vertices;
	int number = 0;
	calculateFinalVertices(8, number, vertices,glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//calculateFinalVertices(14, number, vertices, glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f));

	Shader shaderProgram("vertexShader.vert", "fragmentShader.frag");

	VAO VAO1;
	VAO1.Bind();
	VBO VBO(&vertices[0], vertices.size() * sizeof(float));
	VAO1.LinkAttrib(VBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	shaderProgram.use();

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shaderProgram.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shaderProgram.setMat4("view", view);

		VAO1.Bind();
		glDrawArrays(GL_TRIANGLES, 0, number);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	VAO1.Unbind();
	VBO.Unbind();
	VAO1.Delete();
	VBO.Delete();
	shaderProgram.~Shader();
	glfwTerminate();
}

void calculateFinalVertices(int n, int &verts_number, std::vector<float>& vertices, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	// ta funckja liczy wierzholki wszystkich trojkatow i wklada je od wlasciwego wektora czyli "int &verts_number" 
	std::vector<glm::vec3> v;
	v.push_back(a);
	v.push_back(b);
	v.push_back(c);
	std::vector<glm::vec3> temp;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < v.size(); j += 3)
		{
			// Dla kazdego trojkata liczy jego własne 3 mniejsze trojkaty i zapisuje je w wektorze temp
			calculateVertices(temp, v[j], v[j + 1], v[j + 2]);
		}
		v.clear();
		v = temp;
		temp.clear();
	}

	std::vector<float> final_value;
	for (int i = 0; i < v.size(); i++)
	{
		final_value.push_back(v[i].x);
		final_value.push_back(v[i].y);
		final_value.push_back(v[i].z);
	}

	// wierzholki = 3^(n+1) bo np n = 1 ; v = 9   n = 2; v = 27

	verts_number = pow(3, n + 1);
	vertices = final_value;
}

void calculateVertices(std::vector<glm::vec3>& vertices, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 A, B, C;
	A = a;
	B = b;
	C = c;

	glm::vec3 AB, AC, BC;
	// Liczy punkty na środku powyzszych bokow
	AC = glm::vec3((A.x + C.x) / 2.0f, (A.y + C.y) / 2.0f, 0.0f);
	BC = glm::vec3((B.x + C.x) / 2.0f, (B.y + C.y) / 2.0f, 0.0f);
	AB = glm::vec3((A.x + B.x) / 2.0f, (A.y + B.y) / 2.0f, 0.0f);

	// First triangle
	vertices.push_back(A);
	vertices.push_back(AC);
	vertices.push_back(AB);

	// Second triangle

	vertices.push_back(B);
	vertices.push_back(BC);
	vertices.push_back(AB);

	// Third triangle

	vertices.push_back(C);
	vertices.push_back(AC);
	vertices.push_back(BC);

	// teraz wektor składa się z koordynatów 3 mniejszych trojkatów 
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	static bool lkey = false;

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

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !lkey)
	{
		lkey = true;

		int polygonMode;
		glGetIntegerv(GL_POLYGON_MODE, &polygonMode);

		if (polygonMode == GL_LINE)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (polygonMode == GL_FILL)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && lkey)
		lkey = false;

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
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
