#include "Camera.h"



Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window, inputhandler input, float dt)
{
	this->dt = dt;
	if (input.is_key_pressed(window, GLFW_KEY_W))
	{			
		Position += speed * Orientation * dt;
	}
	if (input.is_key_pressed(window, GLFW_KEY_A))
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up)) * dt;
	}
	if (input.is_key_pressed(window, GLFW_KEY_S))
	{
		Position += speed * -Orientation * dt;
	}
	if (input.is_key_pressed(window, GLFW_KEY_D))
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up)) * dt;
	}
	if (input.is_key_pressed(window, GLFW_KEY_SPACE))
	{
		Position += speed * Up * dt;
	}
	if (input.is_key_pressed(window, GLFW_KEY_LEFT_CONTROL))
	{
		Position += speed * -Up * dt;
	}
	if (input.is_key_pressed(window, GLFW_KEY_LEFT_SHIFT))
	{
		speed = 100.0f;
	}
	else if (!input.is_key_pressed(window, GLFW_KEY_LEFT_SHIFT))
	{
		speed = 5.0f;
	}
	if (input.is_key_pressed(window, GLFW_KEY_ESCAPE))
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		exitProgram = true;
		return;
	}
	if (input.is_key_pressestones(window, GLFW_KEY_F11)) {

		if (fullScreen) {
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
			width = 1200;
			height = 800;
			glViewport(0, 0, width, height);
			glfwSetWindowPos(window, mode->width/2-600, mode->height / 2 - 400);
			glfwSetWindowSize(window, width, height);
			fullScreen = false;
		}
		else {
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
			width = mode->width;
			height = mode->height;
			glViewport(0, 0, width, height);
			glfwSetWindowPos(window, 0, 0);
			glfwSetWindowSize(window, width, height);
			fullScreen = true;
		}
	}

	if (input.switch_key(window, GLFW_KEY_Q)) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		glfwSetCursorPos(window, (width / 2), (height / 2));
	}

	
}

bool Camera::exit() {
	return exitProgram;
}

void Camera::setWinSize(int width, int height)
{
	Camera::width = width;
	Camera::height = height;
}