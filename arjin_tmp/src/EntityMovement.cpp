#include "EntityMovement.hpp"
#include <iostream>
// Constructor/Destructor
EntityMovement::EntityMovement(float maxVel, float accVal, float decVal)
	: m_maxVel(maxVel)
	, m_accVal(accVal)
	, m_decVal(decVal)
{

}

//--Functions
//-Accessors
bool EntityMovement::getState(MovingState state)
{
	switch(state)
	{
	case MovingState::MLeft:
		return (m_vel.x < 0.0f);
		break;
	case MovingState::MRight:
		return (m_vel.x > 0.0f);
		break;
	case MovingState::MUp:
		return (m_vel.y < 0.0f);
		break;
	case MovingState::MDown:
		return (m_vel.y > 0.0f);
		break;
	case MovingState::Moving:
		return (m_vel.x != 0.0f || m_vel.y != 0.0f);
		break;
	case MovingState::Idle:
		return (m_vel.x == 0.0f && m_vel.y == 0.0f);
		break;
	}
	return false;
}

const sf::Vector2f& EntityMovement::getVelocity() const
{
	return m_vel;
}

const float& EntityMovement::getMaxVelocity() const
{
	return m_maxVel;
}

//-Modifiers
void EntityMovement::setVelocity(float x, float y)
{
	m_vel.x = x;
	m_vel.y = y;
}

void EntityMovement::move(float dirx, float diry)
{
	m_vel.x += m_accVal * dirx;
	m_vel.y += m_accVal * diry;
}


//-Core
void EntityMovement::updateVelX()
{
	if(m_vel.x > 0.0f)
	{
		// Max Velocity check
		if(m_vel.x  > m_maxVel)
			m_vel.x = m_maxVel;

		// Deceleration
		m_vel.x -= m_decVal;
		if(m_vel.x  < 0.0f)
			m_vel.x = 0.0f;
	}
	else if(m_vel.x < 0.0f)
	{
		// Max Velocity check
		if(m_vel.x  < -m_maxVel)
			m_vel.x = -m_maxVel;

		// Deceleration
		m_vel.x += m_decVal;
		if(m_vel.x  > 0.0f)
			m_vel.x = 0.0f;
	}
}

void EntityMovement::updateVelY()
{
	if(m_vel.y > 0.0f)
	{
		// Max Velocity check
		if(m_vel.y  > m_maxVel)
			m_vel.y = m_maxVel;

		// Deceleration
		m_vel.y -= m_decVal;
		if(m_vel.y  < 0.0f)
			m_vel.y = 0.0f;
	}
	else if(m_vel.y < 0.0f)
	{
		// Max Velocity check
		if(m_vel.y  < -m_maxVel)
			m_vel.y = -m_maxVel;

		// Deceleration
		m_vel.y += m_decVal;
		if(m_vel.y  > 0.0f)
			m_vel.y = 0.0f;
	}
}

void EntityMovement::update()
{
	// Update velocity
	updateVelX();
	updateVelY();

}

void EntityMovement::apply(sf::Sprite& target, float dt)
{
	target.move(m_vel * dt);
}
