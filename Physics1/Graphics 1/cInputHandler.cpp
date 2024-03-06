
#include "cInputHandler.h"
#include <iostream>
#include <vector>


cInputHandler* cInputHandler::m_pTheInputHandler = nullptr;





void cInputHandler::Initialize(GLFWwindow* window)
{
	cInputHandler::m_pTheInputHandler = new cInputHandler();
	cInputHandler::m_pTheInputHandler->CaptureWindow = window;


	return;
}

cInputHandler* cInputHandler::GetInstance(void)
{
	return cInputHandler::m_pTheInputHandler;
}


bool cInputHandler::IsPressed(unsigned int keyVal)
{
	std::unordered_map<unsigned int, sKey>::iterator mapIT = map_Keystates->find(keyVal);
	if (mapIT == map_Keystates->end()) return false;

	return mapIT->second.isPressed;
}

bool cInputHandler::IsPressedEvent(unsigned int keyVal)
{
	std::unordered_map<unsigned int, sKey>::iterator mapIT = map_Keystates->find(keyVal);
	if (mapIT == map_Keystates->end()) return false;

	return mapIT->second.pressEvent;
}

bool cInputHandler::IsReleasedEvent(unsigned int keyVal)
{
	std::unordered_map<unsigned int, sKey>::iterator mapIT = map_Keystates->find(keyVal);
	if (mapIT == map_Keystates->end()) return false;

	return mapIT->second.releaseEvent;
}

bool cInputHandler::IsMousePressed(unsigned mouseVal)
{
	if (mouseVal == GLFW_MOUSE_BUTTON_LEFT)
	{
		return mouse_States->isLPressed;
	}
	else if (mouseVal == GLFW_MOUSE_BUTTON_RIGHT)
	{
		return mouse_States->isRPressed;
	}

	return false;
}

bool cInputHandler::IsMousePressedEvent(unsigned mouseVal)
{
	if (mouseVal == GLFW_MOUSE_BUTTON_LEFT)
	{
		return mouse_States->LPressEvent;
	}
	else if (mouseVal == GLFW_MOUSE_BUTTON_RIGHT)
	{
		return mouse_States->RPressEvent;
	}

	return false;
}

bool cInputHandler::IsMouseReleasedEvent(unsigned mouseVal)
{
	if (mouseVal == GLFW_MOUSE_BUTTON_LEFT)
	{
		return mouse_States->LReleaseEvent;
	}
	else if (mouseVal == GLFW_MOUSE_BUTTON_RIGHT)
	{
		return mouse_States->RReleaseEvent;
	}
	return false;
}

void cInputHandler::GetMouseDeltas(double& x, double& y)
{
	x = mouse_States->deltaX;
	y = mouse_States->deltaY;

	return;
}

void cInputHandler::ChangeMouseState(MouseStates newState)
{
	mouse_States->mouseType = newState;
	if (newState == NORMAL)
	{
		//glfwSetCursorPos(m_window, width / 2, height / 2);
		glfwSetInputMode(CaptureWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		glfwSetInputMode(CaptureWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void cInputHandler::Update()
{
	GetMonitor(); // Updates monitor info (width x height)
	GetKeys();
	GetMouse();

	// Now get mouse data

}

// Keys referenced from: https://www.glfw.org/docs/3.3/group__keys.html
cInputHandler::cInputHandler()
{
	sKey defKey;
	map_Keystates = new std::unordered_map<unsigned int, sKey>();
	// Insert all keys
	map_Keystates->insert(std::pair<unsigned int, sKey>(32, defKey));
	map_Keystates->insert(std::pair<unsigned int, sKey>(39, defKey));

	for (unsigned int i = 44; i < 58; i++)
	{
		map_Keystates->insert(std::pair<unsigned int, sKey>(i, defKey));
	}
	map_Keystates->insert(std::pair<unsigned int, sKey>(59, defKey));
	map_Keystates->insert(std::pair<unsigned int, sKey>(61, defKey));

	for (unsigned int i = 65; i < 94; i++)
	{
		map_Keystates->insert(std::pair<unsigned int, sKey>(i, defKey));
	}

	map_Keystates->insert(std::pair<unsigned int, sKey>(96, defKey));
	map_Keystates->insert(std::pair<unsigned int, sKey>(161, defKey));
	map_Keystates->insert(std::pair<unsigned int, sKey>(162, defKey));

	for (unsigned int i = 256; i < 270; i++)
	{
		map_Keystates->insert(std::pair<unsigned int, sKey>(i, defKey));
	}
	for (unsigned int i = 280; i < 285; i++)
	{
		map_Keystates->insert(std::pair<unsigned int, sKey>(i, defKey));
	}
	for (unsigned int i = 290; i < 315; i++)
	{
		map_Keystates->insert(std::pair<unsigned int, sKey>(i, defKey));
	}
	for (unsigned int i = 320; i < 337; i++)
	{
		map_Keystates->insert(std::pair<unsigned int, sKey>(i, defKey));
	}
	for (unsigned int i = 340; i < 349; i++)
	{
		map_Keystates->insert(std::pair<unsigned int, sKey>(i, defKey));
	}

	// Initialize Mouse
	mouse_States = new sMouse();

	// Initialize Monitor
	monitor_Info = new sMonitorInfo();
}

void cInputHandler::GetKeys(void)
{
	for (std::unordered_map<unsigned int, sKey>::iterator mapIT = this->map_Keystates->begin(); mapIT != this->map_Keystates->end(); mapIT++)
	{
		sKey* currState = &mapIT->second; // So we can just edit the easy to read currState
		int actualKeyState = glfwGetKey(CaptureWindow, mapIT->first);

		if (actualKeyState) // Key pressed
		{
			if (currState->isPressed)
			{
				currState->pressEvent = false;
			}
			else
			{
				currState->pressEvent = true;
				currState->isPressed = true;
			}
		}
		else // Key not pressed
		{
			if (currState->isPressed)
			{
				currState->isPressed = false;
				currState->releaseEvent = true;
			}
			else
			{
				currState->releaseEvent = false;
			}
		}
	}
}

void cInputHandler::GetMouse(void)
{
	int lClick = glfwGetMouseButton(CaptureWindow, GLFW_MOUSE_BUTTON_LEFT);
	int rClick = glfwGetMouseButton(CaptureWindow, GLFW_MOUSE_BUTTON_RIGHT);

	if (lClick) // Pressed
	{
		if (mouse_States->isLPressed) // Been pressing
		{
			mouse_States->LPressEvent = false;
		}
		else // First time press
		{
			mouse_States->LPressEvent = true;
			mouse_States->isLPressed = true;
		}
	}
	else // Not pressed
	{
		if (mouse_States->isLPressed) // First release
		{
			mouse_States->isLPressed = false;
			mouse_States->LReleaseEvent = true;
		}
		else // Been releasing
		{
			mouse_States->LReleaseEvent = false;
		}
	}
	if (rClick) // Pressed
	{
		if (mouse_States->isRPressed) // Been pressing
		{
			mouse_States->RPressEvent = false;
		}
		else // First time press
		{
			mouse_States->RPressEvent = true;
			mouse_States->isRPressed = true;
		}
	}
	else // Not pressed
	{
		if (mouse_States->isRPressed) // First release
		{
			mouse_States->isRPressed = false;
			mouse_States->RReleaseEvent = true;
		}
		else // Been releasing
		{
			mouse_States->RReleaseEvent = false;
		}
	}


	// Now for mouse movement updates


	// Update previous pos before getting new one
	mouse_States->prevX = mouse_States->currX;
	mouse_States->prevY = mouse_States->currY;

	// New mouse pos
	glfwGetCursorPos(CaptureWindow, &mouse_States->currX, &mouse_States->currY);

	//std::cout << mouse_States->currX << std::endl;

	// Update deltas
	mouse_States->deltaX = mouse_States->currX - mouse_States->prevX;
	mouse_States->deltaY = mouse_States->currY - mouse_States->prevY;

	if (mouse_States->mouseType == HIDDEN)
	{
		glfwSetCursorPos(CaptureWindow, static_cast<float>(monitor_Info->width) / 2, static_cast<float>(monitor_Info->height) / 2);
		mouse_States->currX = static_cast<float>(monitor_Info->width) / 2;
		mouse_States->currY = static_cast<float>(monitor_Info->height) / 2;
	}


}

void cInputHandler::GetMonitor(void)
{
	glfwGetFramebufferSize(CaptureWindow, &monitor_Info->width, &monitor_Info->height);
}

// void cInputHandler::queryKeys(GLFWwindow* window)
// {
// 
// 	std::vector<bool> keysPressed;
// 	int state = glfwGetKey(window, GLFW_KEY_W);
// 	if (state == GLFW_PRESS)
// 		keysPressed.push_back(true);
// 	else
// 		keysPressed.push_back(false);
// 	state = glfwGetKey(window, GLFW_KEY_S);
// 	if (state == GLFW_PRESS)
// 		keysPressed.push_back(true);
// 	else
// 		keysPressed.push_back(false);
// 	state = glfwGetKey(window, GLFW_KEY_A);
// 	if (state == GLFW_PRESS)
// 		keysPressed.push_back(true);
// 	else
// 		keysPressed.push_back(false);
// 	state = glfwGetKey(window, GLFW_KEY_D);
// 	if (state == GLFW_PRESS)
// 		keysPressed.push_back(true);
// 	else
// 		keysPressed.push_back(false);
// 
// 
// 	state = glfwGetKey(window, GLFW_KEY_UP);
// 	if (state == GLFW_PRESS)
// 		keysPressed.push_back(true);
// 	else
// 		keysPressed.push_back(false);
// 	state = glfwGetKey(window, GLFW_KEY_DOWN);
// 	if (state == GLFW_PRESS)
// 		keysPressed.push_back(true);
// 	else
// 		keysPressed.push_back(false);
// 	state = glfwGetKey(window, GLFW_KEY_LEFT);
// 	if (state == GLFW_PRESS)
// 		keysPressed.push_back(true);
// 	else
// 		keysPressed.push_back(false);
// 	state = glfwGetKey(window, GLFW_KEY_RIGHT);
// 	if (state == GLFW_PRESS)
// 		keysPressed.push_back(true);
// 	else
// 		keysPressed.push_back(false);
// 
// 	state = glfwGetKey(window, GLFW_KEY_UP);
// 	if (state == GLFW_PRESS)
// 		keysPressed.push_back(true);
// 	else
// 		keysPressed.push_back(false);
// 
// }
