
#include "cTimer.h"


cTimer* cTimer::m_pTheController = nullptr;
unsigned int cTimer::m_NextID;
std::unordered_map<unsigned int, sTimer*> cTimer::m_mapIDtoTimer;




cTimer* cTimer::GetInstance(void)
{
	if (cTimer::m_pTheController == nullptr)
	{
		cTimer::m_pTheController = new cTimer();
		cTimer::m_pTheController->Initialize();
	}
	return cTimer::m_pTheController;
}

void cTimer::Initialize(void) // This should only be called one per instance of program!!!
{
	sTimer* newtimer = new sTimer();
	cTimer::m_NextID = 0;

}

void cTimer::Update(double dt)
{
	for (std::unordered_map<unsigned int, sTimer*>::iterator mapIT = m_mapIDtoTimer.begin();
		mapIT != m_mapIDtoTimer.end();
		mapIT++)
	{
		mapIT->second->Update(dt);
	}

	return;
}

sTimer* cTimer::MakeNewTimer(float interval, sTimer::eTimerType type)
{
	if (interval <= 0.0f) return nullptr;

	sTimer* newTimer = new sTimer();
	newTimer->type = type;
	newTimer->INTERVAL = interval;
	newTimer->timeTillNextInterval = interval;
	newTimer->ID = cTimer::m_NextID++;

	cTimer::m_mapIDtoTimer[newTimer->ID] = newTimer;

	return newTimer;
}
