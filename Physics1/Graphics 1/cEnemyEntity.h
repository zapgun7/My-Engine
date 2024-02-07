#pragma once

struct sPhysicsProperties;



enum eAIType
{
	SEEK,
	FLEE,
	PURSUE,
	EVADE,
	APPROACH
	
};


class cEnemyEntity
{
public:
	cEnemyEntity();
	~cEnemyEntity();

	void Update(double deltaTime);


	void setTargetObject(sPhysicsProperties* goalObj);

private:
	sPhysicsProperties* m_pEntityObject = nullptr;
	sPhysicsProperties* m_pPlayerEntity = nullptr; // Do it this way, or pass target object every update


	const float MAXMOVESPEED = 20.0f;
	const float MAXTURNSPEED = 5.0f;

	eAIType m_eType = SEEK;

};