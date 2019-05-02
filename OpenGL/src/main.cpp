#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shaders.h>
#include <camera.h>
#include <Object.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <Raycast.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <map>

#include <data.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
float range = 12.f;
int floorSize = 3;

// camera
Camera camera(glm::vec3(2, 2, 5));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//cursor
bool cursorActivated = false;
float lastCursorEnable = 0.f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float deltaActionTime = 2.f;
float lastActionTime = 0.0f;
float lastFrame = 0.0f;
float minActionTime = 0.2f;

//objects
std::map<std::string, Object> objs;
int blockIndex = 0;

glm::vec4 randColor() {
	float rand1 = (float)(rand() % 256) / 256;
	float rand2 = (float)(rand() % 256) / 256;
	float rand3 = (float)(rand() % 256) / 256;
	return glm::vec4(rand1, rand2, rand3, 1);
}


int main()
{
	
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 64);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader program
	// ------------------------------------
	Shader shader("C:/Dev/OpenGL/OpenGL/src/vshader.glsl", "C:/Dev/OpenGL/OpenGL/src/fshader.glsl"); // you can name your shader files however you like

	glEnable(GL_DEPTH_TEST);
	glLineWidth(2.f);
	glEnable(GL_MULTISAMPLE);

	srand(time(0));

	Object leftWall = Object(groundPlaneVerts, glm::vec3(-0.5, 2, 2), glm::vec4(1, 0.2, 0.2, 1));
	leftWall.scale = glm::vec3(5);
	leftWall.rotation = glm::vec3(0, 1, 0);
	leftWall.rotationAngle = glm::radians(90.f);
	Object rightWall = leftWall;
	rightWall.position = glm::vec3(2, 2, -0.5);
	rightWall.rotationAngle = 0;
	rightWall.color = glm::vec4(0.2, 1, 0.2, 1);
	Object groundWall = Object(groundPlaneVerts, glm::vec3(2, -0.5, 2), glm::vec4(0.2, 0.2, 1, 1));
	groundWall.rotation = glm::vec3(1, 0, 0);
	groundWall.rotationAngle = glm::radians(90.f);
	groundWall.scale = glm::vec3(5);

	objs["leftWall"] = leftWall;
	objs["rightWall"] = rightWall;
	objs["groundWall"] = groundWall;

	for (int i = 0; i < 5; i++) {
		Object temp = Object(cubeVerts, glm::vec3(0, i, 0), randColor());
		objs["block" + blockIndex] = temp;
		blockIndex++;
	}

	Object cursor = Object(cubeVerts, glm::vec3(0, 0, -8), glm::vec4(0, 1, 0, 1), glm::vec4(1, 1, 0.5, 1), DM_ONLY_LINES);
	cursor.scale = glm::vec3(0.2);
	objs["cursor"] = cursor;
	
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		deltaActionTime = currentFrame - lastActionTime;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// activate shader
		shader.use();

		//lighting stuff
		shader.setVec4("lightColor", glm::vec4(1, 1, 1, 1));
		shader.setVec3("lightPos", glm::vec3(5, 5, 5));
		shader.setVec3("viewPos", camera.Position);

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
		shader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
		
		for (std::map<std::string, Object>::iterator it = objs.begin(); it != objs.end(); it++) {
			RaycastHit raycastHit;
			if (it->first.substr(0, 5) == "block") {
				if (glm::length(it->second.position - camera.Position) > range)
					continue;
				if (Raycast(camera.Position, camera.Front, objs, range, raycastHit)) {
					objs["cursor"].position = camera.Position + camera.Front * raycastHit.distance * 0.99f;
					objs["cursor"].lineColor = glm::vec4(1, 0, 0, 1);
					objs["cursor"].scale = glm::vec3(0.2, 0.2, 0.2);
				}
				else {
					objs["cursor"].position = camera.Position + camera.Front * range;
					objs["cursor"].lineColor = glm::vec4(1, 1, 0.5, 1);
				}
			}
			if (objs.size() <= 4) {
				objs["cursor"].position = camera.Position + camera.Front * range;
				objs["cursor"].lineColor = glm::vec4(1, 1, 0.5, 1);
			}
		}

		//draw calls
		for (std::map<std::string, Object>::iterator it = objs.begin(); it != objs.end(); it++)
				it->second.Draw(shader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		RaycastHit raycastHit;
		if (Raycast(camera.Position, camera.Front, objs, range, raycastHit)) {
			if (raycastHit.key != "" && deltaActionTime > minActionTime && raycastHit.key.substr(0, 5) == "block") {
				objs.erase(raycastHit.key);
				lastActionTime = glfwGetTime();
			}
		}
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		if (deltaActionTime > minActionTime) {
			glm::vec3 cp = objs["cursor"].position;
			for (int i(0); i < 3; i++) cp[i] = round(cp[i]);
			objs["block" + std::to_string(blockIndex++)] = Object(cubeVerts, cp, randColor());
			lastActionTime = glfwGetTime();
		}
	}

	//mouse capture toggle
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && glfwGetTime() - lastCursorEnable > 0.5) {
		if (cursorActivated) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		lastX = SCR_WIDTH / 2.0f;
		lastY = SCR_HEIGHT / 2.0f;
		glfwSetCursorPos(window, lastX, lastY);
		cursorActivated = !cursorActivated;
		lastCursorEnable = glfwGetTime();
	}		

	//Camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (cursorActivated)
		return;
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