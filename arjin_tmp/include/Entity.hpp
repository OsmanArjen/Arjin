#ifndef ENTITY_HPP
#define ENTITY_HPP
#include "EntityMovement.hpp"
#include "EntityAnimation.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

//class EntityHitbox;
//class EntityAnimation;
class Entity: public sf::Drawable
{
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
protected: 
	sf::Sprite m_sprite;
	
	//::nullptr is default value 
	EntityMovement*  m_movement; // use the pointer if the object created
//	EntityHitbox*    m_hitbox;   // use the pointer if the object created
	EntityAnimation* m_anim;     // use the pointer if the object created
public:
	//--Constructor/Destructor
	Entity();
	virtual ~Entity() = default;

	//--Functions
	//-Getter/Setter
	virtual const sf::Vector2f& getPosition();
	virtual void setPosition(const sf::Vector2f& pos);

	//-Modifiers
	virtual void setVelocity(float x, float y);
	virtual void move(float dirx, float diry);

	//-Core
	virtual void update(float dt) = 0;
	
}; 

#endif // ENTITY_HPP