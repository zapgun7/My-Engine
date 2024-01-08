#include "cCommand_MoveToBezier.h"
#include <iostream>

cCommand_MoveToBezier::cCommand_MoveToBezier()
{

}

cCommand_MoveToBezier::~cCommand_MoveToBezier()
{

}

void cCommand_MoveToBezier::Initialize(void* initStruct)
{
	initBezMoveInfo* info = (initBezMoveInfo*)initStruct;

	this->m_startXYZ = info->startPos;
	this->m_endXYZ = info->destPos;
	this->m_TimeToMove = info->timeInSeconds;
	this->m_pThePhysObj = info->theObj;
	this->bezOffset = info->bezOffset;
	this->rUT = info->rampUpTime;
	this->rDT = info->rampDownTime;

	// Now to calculate
	this->m_direction = glm::normalize(m_endXYZ - m_startXYZ);


	this->m_state = BEZRAMPUP;
}

bool cCommand_MoveToBezier::Update(double deltaTime)
{
	if (this->m_pThePhysObj == nullptr)
	{
		m_state == BEZDONE;
		return true;
	}

	if (m_elapsedTime == 0)
	{
		PreStart();
	}
	// I'm deciding that when the time is up, I'm done.

	// Update the time
	this->m_elapsedTime += deltaTime;

	if (m_state == BEZRAMPUP)
	{
		//m_pThePhysObj->position = m_startXYZ + (float)(m_elapsedTime * m_elapsedTime) * m_direction * rUSpd;
		internalFauxPos = m_startXYZ + (float)(m_elapsedTime * m_elapsedTime) * m_direction * rUSpd;

		if (m_elapsedTime >= rUT)
		{
			// Set where it should be here
			//m_pThePhysObj->position = steadyStartPos;//m_startXYZ + (float)(rUT * rUT) * m_direction * rUSpd; 
			internalFauxPos = steadyStartPos;

			// Make up overflowed time
			float overflowTime = m_elapsedTime - rUT;
			//m_pThePhysObj->position = steadyStartPos + m_direction * m_MaxSpeed * overflowTime;
			internalFauxPos = steadyStartPos + m_direction * m_MaxSpeed * overflowTime;
			m_state = BEZSTEADY;
		}
	}
	else if (m_state == BEZSTEADY)
	{
		//m_pThePhysObj->position = steadyStartPos + (float)(m_elapsedTime - rUT) * m_direction * m_MaxSpeed;
		internalFauxPos = steadyStartPos + (float)(m_elapsedTime - rUT) * m_direction * m_MaxSpeed;

		if (m_elapsedTime >= m_TimeToMove - rDT)
		{
			//m_pThePhysObj->position = rampDownStartPos;
			internalFauxPos = rampDownStartPos;

			// Make up for time
			float overflowTime = m_elapsedTime - (m_TimeToMove - rDT);
			//m_pThePhysObj->position = rampDownStartPos + ((m_direction * rDSpd * overflowTime) + m_MaxSpeed) * overflowTime;
			internalFauxPos = rampDownStartPos + ((m_direction * rDSpd * overflowTime) + m_MaxSpeed) * overflowTime;
			m_state = BEZRAMPDOWN;
		}
	}
	else if (m_state == BEZRAMPDOWN)
	{
		float timePassed = m_elapsedTime - (m_TimeToMove - rDT);
		//m_pThePhysObj->position = rampDownStartPos + m_direction * ((rDSpd * timePassed) + m_MaxSpeed) * timePassed;
		internalFauxPos = rampDownStartPos + m_direction * ((rDSpd * timePassed) + m_MaxSpeed) * timePassed;

		if (m_elapsedTime >= m_TimeToMove)
		{
			//m_pThePhysObj->position = m_endXYZ;
			internalFauxPos = m_endXYZ;
			m_state = BEZDONE;
		}
	}

	// Calculate bezier position here
	float ratio = glm::length(internalFauxPos - m_startXYZ) / glm::length(m_endXYZ - m_startXYZ);

	

	glm::vec3 lineAPos = AtoBez * ratio;
	glm::vec3 lineBPos = BeztoB * ratio;

	

	lineAPos += m_startXYZ;
	lineBPos += bezierPoint;

	// Find line between these two points
	glm::vec3 thePoint = lineBPos - lineAPos;
	thePoint *= ratio;

	thePoint += lineAPos;
	
	m_pThePhysObj->position = thePoint;

	return this->isDone();
}

// Also called every frame. Returns true when done
bool cCommand_MoveToBezier::isDone(void)
{
	// 	if (this->m_elapsedTime >= this->m_TimeToMove )
	// 	{
	// 		// Yes
	// 		return true;
	// 	}
	// 	// Nope
	// 	return false;
	if (this->m_state == BEZDONE)
	{
		return true;
	}
	return false;
}

bool cCommand_MoveToBezier::PreStart(void)
{
	// Re-initialize variables
	m_startXYZ = m_pThePhysObj->position;

	m_distance = glm::length(m_endXYZ - m_startXYZ);
	this->m_direction = glm::normalize(m_endXYZ - m_startXYZ);

	m_MaxSpeed = m_distance / (m_TimeToMove - rUT * 0.5f - rDT * 0.5f);

	// Calculate constant accel/deccel 
	rUSpd = m_MaxSpeed / rUT;
	rDSpd = -m_MaxSpeed / rDT;

	rUSpd /= 2;
	rDSpd /= 2;


	steadyStartPos = m_startXYZ + rUT * rUT * m_direction * rUSpd;
	rampDownStartPos = steadyStartPos + m_MaxSpeed * m_direction * (m_TimeToMove - rUT - rDT);


	// New Bezier calculations

	// Want to start by getting the right vector and the up vector of our direction (relative to it)
	glm::vec3 rightVec = -glm::normalize(glm::cross(glm::vec3(0, 1, 0), m_direction));
	glm::vec3 upVec = -glm::normalize(glm::cross(m_direction, rightVec)); 

	bezierPoint = m_startXYZ + m_endXYZ * 0.5f; // Get halfway point for now, will make it customizable later

	// Calculate the point we'll generally curve towards
	bezierPoint += rightVec * bezOffset.x;
	bezierPoint += upVec * bezOffset.y;

	// Calc these two so we can calculate our position on the curve
	AtoBez = bezierPoint - m_startXYZ;
	BeztoB = m_endXYZ - bezierPoint;

	// How the main loop will work, same as before with the smooth linear movement
	// But now we'll calculate the % traversal of the original line and sum that % of the two lines just calculated to get our shiiittt oh yeah



	m_state = BEZRAMPUP;

	return false;
}

bool cCommand_MoveToBezier::PostEnd(void)
{

	return false;
}

