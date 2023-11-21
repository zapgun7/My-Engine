
#include "cInputHandler.h"
#include <iostream>
#include <vector>

cInputHandler::cInputHandler()
{
	//theArena = cArena::getArena();
}

void cInputHandler::queryKeys(GLFWwindow* window)
{

	std::vector<bool> keysPressed;
	int state = glfwGetKey(window, GLFW_KEY_W);
	if (state == GLFW_PRESS)
		keysPressed.push_back(true);
	else
		keysPressed.push_back(false);
	state = glfwGetKey(window, GLFW_KEY_S);
	if (state == GLFW_PRESS)
		keysPressed.push_back(true);
	else
		keysPressed.push_back(false);
	state = glfwGetKey(window, GLFW_KEY_A);
	if (state == GLFW_PRESS)
		keysPressed.push_back(true);
	else
		keysPressed.push_back(false);
	state = glfwGetKey(window, GLFW_KEY_D);
	if (state == GLFW_PRESS)
		keysPressed.push_back(true);
	else
		keysPressed.push_back(false);


	state = glfwGetKey(window, GLFW_KEY_UP);
	if (state == GLFW_PRESS)
		keysPressed.push_back(true);
	else
		keysPressed.push_back(false);
	state = glfwGetKey(window, GLFW_KEY_DOWN);
	if (state == GLFW_PRESS)
		keysPressed.push_back(true);
	else
		keysPressed.push_back(false);
	state = glfwGetKey(window, GLFW_KEY_LEFT);
	if (state == GLFW_PRESS)
		keysPressed.push_back(true);
	else
		keysPressed.push_back(false);
	state = glfwGetKey(window, GLFW_KEY_RIGHT);
	if (state == GLFW_PRESS)
		keysPressed.push_back(true);
	else
		keysPressed.push_back(false);

	state = glfwGetKey(window, GLFW_KEY_UP);
	if (state == GLFW_PRESS)
		keysPressed.push_back(true);
	else
		keysPressed.push_back(false);

}
