#pragma once

#include <vector>
#include <unordered_map>


// This class give access to multiple kinds of timers
// One-off timers, where a time is set and it returns true once that time is up
// Repeat timers, where an interval is set and every update after that interval passes, true is returned
// In both cases, it stays true until checked
// User creates objects of this class by calling static
// 

// An individual timer
struct sTimer
{
	enum eTimerType
	{
		ONEOFF,
		REPEAT
	};
	eTimerType type = ONEOFF;

	float INTERVAL = 0;
	float timeTillNextInterval = 0;
	bool intervalHit = false;
	bool isStopped = false;
	unsigned int ID = 0;


	void Update(double dt) // Should only be called by the Controller
	{
		if (isStopped) return;
		timeTillNextInterval -= static_cast<float>(dt);
		if (timeTillNextInterval <= 0)
		{
			timeTillNextInterval += INTERVAL;
			intervalHit = true;
			if (type == ONEOFF) isStopped = true;
		}
	}

	bool CheckInterval(void)
	{
		if (intervalHit)
		{
			// Switch it off
			intervalHit = false;
			return true;
		}
		return false;
	}

	void Reset(void)
	{
		timeTillNextInterval = INTERVAL;
		intervalHit = false;
		isStopped = false;
	}
};

class cTimer
{
public:
	static cTimer* GetInstance(void);
	void Initialize(void); 
	void Update(double dt);

	sTimer* MakeNewTimer(float interval, sTimer::eTimerType type);



private:
	static cTimer* m_pTheController; // Created and updated by the engineController
	static unsigned int m_NextID;
	//std::vector<sTimer*> m_vecTimers;
	std::unordered_map<unsigned int, sTimer*> m_mapIDtoTimer;

};