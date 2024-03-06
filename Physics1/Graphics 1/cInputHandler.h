#pragma once
#include "Other Graphics Stuff/OpenGLCommon.h"
#include <map>
#include <unordered_map>


enum MouseStates
{
	NORMAL,
	HIDDEN // Invis mouse, sets cursor to center of screen
};



struct sKey
{
	bool isPressed = false;
	bool pressEvent = false;
	bool releaseEvent = false;
};

struct sMouse
{
	bool isLPressed = false;
	bool LPressEvent = false;
	bool LReleaseEvent = false;

	bool isRPressed = false;
	bool RPressEvent = false;
	bool RReleaseEvent = false;

	double prevX = 0.0f;
	double currX = 0.0f;
	double deltaX = 0.0f;

	double prevY = 0.0f;
	double currY = 0.0f;
	double deltaY = 0.0f;

	MouseStates mouseType = NORMAL;

};

struct sMonitorInfo
{
	int height = 0.0f;
	int width = 0.0f;
};

class cInputHandler
{
public:
	static void Initialize(GLFWwindow* window);
	static cInputHandler* GetInstance(void);

	bool IsPressed(unsigned int keyVal);
	bool IsPressedEvent(unsigned int keyVal);
	bool IsReleasedEvent(unsigned int keyVal);

	bool IsMousePressed(unsigned mouseVal);
	bool IsMousePressedEvent(unsigned mouseVal);
	bool IsMouseReleasedEvent(unsigned mouseVal);

	void GetMouseDeltas(double& x, double& y);

	void ChangeMouseState(MouseStates newState);

	void Update();
	//void queryKeys(GLFWwindow* window);

private:
	cInputHandler();


	void GetKeys(void);
	void GetMouse(void);
	void GetMonitor(void);


	
	//std::map<unsigned int, sKey>* map_Keystates; // bool1 = isPressed   pair2 bool1 = pressEvent  pair2 bool2 = releaseEvent
	std::unordered_map<unsigned int, sKey>* map_Keystates;

	sMouse* mouse_States;
	
	sMonitorInfo* monitor_Info;

	static cInputHandler* m_pTheInputHandler;
	GLFWwindow* CaptureWindow;
};