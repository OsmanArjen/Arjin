#include "Entity.hpp"
// Draw wrapper for SF::Drawable
void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// High level
	target.draw(m_sprite, states);
}
//--Constructor/Destructor
Entity::Entity()
	: m_movement(nullptr)
//	, m_hitbox(nullptr)
	, m_anim(nullptr)
{

}

//--Functions
//-Getter/Setter
const sf::Vector2f& Entity::getPosition()
{
	return m_sprite.getPosition();

}

void Entity::setPosition(const sf::Vector2f& pos)
{
	m_sprite.setPosition(pos);
}

//-Modifiers

void Entity::setVelocity(float x, float y)
{
	if(m_movement)
		m_movement->setVelocity(x, y);
}

void Entity::move(float dirx, float diry)
{
	if(m_movement)
		m_movement->move(dirx, diry);
}
