#ifndef PLAYER_HPP
#define PLAYER_HPP 
#include "Entity.hpp"
#include <cmath>
class Player: public Entity
{
private:
	enum class CharDir {Left, Right, Up, Down};
	sf::Texture m_spriteSheet;
	sf::IntRect m_idleRectLeft,  m_idleRectRight;
	sf::IntRect m_idleRectUp,    m_idleRectDown;
	CharDir m_dir;
	// Init functions
	void initAttributes();
	void initAnimations();
public:
	//--Constructor/Destructor
	Player(const sf::Vector2f& pos);
	~Player();

	//--Functions
	void updateAnims(float dt);
	//-Core
	void update(float dt);

};

#endif