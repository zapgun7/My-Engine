#include "cCommand_Orient.h"


cCommand_Orient::cCommand_Orient()
{

}

cCommand_Orient::~cCommand_Orient()
{

}


void cCommand_Orient::Initialize(void* initStruct)
{
	initOrientInfo* info = (initOrientInfo*)initStruct;

	this->m_startOriEuler = info->startOri;
	this->m_endOriEuler = info->destOri;
	this->m_TimeToMove = info->timeInSeconds;
	this->m_pThePhysObj = info->theObj;

	this->rUT = info->rampUpTime;
	this->rDT = info->rampDownTime;

	// Now to calculate
	this->m_direction = glm::normalize(m_endOriEuler - m_startOriEuler);
// 	this->m_speed = glm::length(m_endOriEuler - m_startOriEuler) / m_TimeToMove;
// 	this->m_RotateVelocity = m_RotateDir * m_speed;
}

bool cCommand_Orient::Update(double deltaTime)
{
	if (this->m_pThePhysObj == nullptr)
	{
		m_state == ORIDONE;
		return true;
	}

	if (m_elapsedTime == 0)
	{
		PreStart();
	}
	// I'm deciding that when the time is up, I'm done.

	// Update the time
	this->m_elapsedTime += deltaTime;

	if (m_state == ORIRAMPUP)
	{
		m_pThePhysObj->setRotationFromEuler(m_startOriEuler + (float)(m_elapsedTime * m_elapsedTime) * m_direction * rUSpd);

		if (m_elapsedTime >= rUT)
		{
			// Set where it should be here
			m_pThePhysObj->setRotationFromEuler(steadyStartOri);

			// Make up overflowed time
			float overflowTime = m_elapsedTime - rUT;
			m_pThePhysObj->setRotationFromEuler(steadyStartOri + m_direction * m_MaxSpeed * overflowTime);//position = steadyStartPos + m_direction * m_MaxSpeed * overflowTime;
			m_state = ORISTEADY;
		}
	}
	else if (m_state == ORISTEADY)
	{
		m_pThePhysObj->setRotationFromEuler(steadyStartOri + (float)(m_elapsedTime - rUT) * m_direction * m_MaxSpeed);//position = steadyStartPos + (float)(m_elapsedTime - rUT) * m_direction * m_MaxSpeed;

		if (m_elapsedTime >= m_TimeToMove - rDT)
		{
			m_pThePhysObj->setRotationFromEuler(rampDownStartOri);// ->position = rampDownStartPos;

			// Make up for time
			float overflowTime = m_elapsedTime - (m_TimeToMove - rDT);
			m_pThePhysObj->setRotationFromEuler(rampDownStartOri + ((m_direction * rDSpd * overflowTime) + m_MaxSpeed) * overflowTime);// ->position = rampDownStartPos + ((m_direction * rDSpd * overflowTime) + m_MaxSpeed) * overflowTime;
			m_state = ORIRAMPDOWN;
		}
	}
	else if (m_state == ORIRAMPDOWN)
	{
		float timePassed = m_elapsedTime - (m_TimeToMove - rDT);
		m_pThePhysObj->setRotationFromEuler(rampDownStartOri + m_direction * ((rDSpd * timePassed) + m_MaxSpeed) * timePassed);// ->position = rampDownStartPos + m_direction * ((rDSpd * timePassed) + m_MaxSpeed) * timePassed;

		if (m_elapsedTime >= m_TimeToMove)
		{
			m_pThePhysObj->setRotationFromEuler(m_endOriEuler);//->position = m_endXYZ;
			m_state = ORIDONE;
		}
	}



// 	//glm::vec3 currentEulerOri = this->m_pThePhysObj->get_eOrientation();
// 
// 	//currentXYZ += (this->m_velocity * (float)deltaTime);
// 
// 	//this->m_pThePhysObj->position = currentXYZ;
// 	glm::quat thaQuat = glm::quat(glm::radians((this->m_RotateVelocity * (float)deltaTime)));
// 	//this->m_pThePhysObj->addRotationFromQuat(m_qRotationalVelocity);// *(float)deltaTime);
// 	this->m_pThePhysObj->addRotationFromQuat(thaQuat);// *(float)deltaTime);


	return this->isDone();
}

// Also called every frame. Returns true when done
bool cCommand_Orient::isDone(void)
{
// 	if (this->m_elapsedTime >= this->m_TimeToMove)
// 	{
// 		// Yes
// 		return true;
// 	}
// 	// Nope
// 	return false;

	return (m_state == ORIDONE);
}

bool cCommand_Orient::PreStart(void)
{
	// Re-initialize variables
// 	m_startOriEuler = m_pThePhysObj->get_eOrientation();
// 	this->m_RotateDir = glm::normalize(m_endOriEuler - m_startOriEuler);
// 	this->m_speed = glm::length(m_endOriEuler - m_startOriEuler) / m_TimeToMove;
// 	this->m_RotateVelocity = m_RotateDir * m_speed;
// 	m_qRotationalVelocity = glm::quat(glm::radians(m_RotateVelocity));
// 	return false;

	// Re-initialize variables
	//m_startOriEuler = m_pThePhysObj->get_eOrientation();

	m_distance = glm::length(m_endOriEuler - m_startOriEuler);
	this->m_direction = glm::normalize(m_endOriEuler - m_startOriEuler);

	m_MaxSpeed = m_distance / (m_TimeToMove - rUT * 0.5f - rDT * 0.5f);

	// Calculate constant accel/deccel 
	rUSpd = m_MaxSpeed / rUT;
	rDSpd = -m_MaxSpeed / rDT;

	rUSpd /= 2;
	rDSpd /= 2;


	steadyStartOri = m_startOriEuler + rUT * rUT * m_direction * rUSpd;
	rampDownStartOri = steadyStartOri + m_MaxSpeed * m_direction * (m_TimeToMove - rUT - rDT);



	m_state = ORIRAMPUP;

	return false;
}

bool cCommand_Orient::PostEnd(void)
{

	return false;
}

