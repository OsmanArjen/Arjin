#include "Player.hpp"

// Init functions
void Player::initAttributes()
{
	//-Init attributes
	m_sprite.setScale(2.0f, 2.0f);
	//--Init resources
	m_spriteSheet.loadFromFile("../res/assets/devChar.png");
	//--Init components
	m_movement = new EntityMovement(300.0f, 15.0f, 7.5f);
	m_anim     = new EntityAnimation(m_sprite, m_spriteSheet);
}

void Player::initAnimations()
{

	m_anim->add("walk_up",    {1,0}, {8,0}, {64, 64}, 12.0f);
	m_anim->add("walk_down",  {1,2}, {8,2}, {64, 64}, 12.0f);
	m_anim->add("walk_left",  {1,1}, {8,1}, {64, 64}, 12.0f);
	m_anim->add("walk_right", {1,3}, {8,3}, {64, 64}, 12.0f);
}

//--Constructor/Destructor
Player::Player(const sf::Vector2f& pos)
	: m_idleRectLeft ({0, 64, 64,64})
	, m_idleRectRight({0, 192,64,64})
	, m_idleRectUp   ({0, 0,  64,64})
	, m_idleRectDown ({0, 128,64,64})
	, m_dir(CharDir::Right)

{
	// Initialize attributes
	initAttributes();
	initAnimations();
	setPosition(pos);
}

Player::~Player()
{
	delete m_movement;
//	delete m_hitbox;
	delete m_anim;
}

//--Functions
void Player::updateAnims(float dt)
{
	if(m_movement->getState(MovingState::Idle))
	{
		m_anim->resetLastAnim();
		switch (m_dir)
		{
			case CharDir::Left:
				m_sprite.setTextureRect(m_idleRectLeft);
				break;
			case CharDir::Right:
				m_sprite.setTextureRect(m_idleRectRight);
				break;
			case CharDir::Up:
				m_sprite.setTextureRect(m_idleRectUp);
				break;
			case CharDir::Down:
				m_sprite.setTextureRect(m_idleRectDown);
				break;
		}
			
	}
	else if(m_movement->getState(MovingState::MLeft))
	{
		m_anim->play("walk_left", dt, std::abs(m_movement->getVelocity().x / m_movement->getMaxVelocity()) );
		m_dir = CharDir::Left;
	}

	else if (m_movement->getState(MovingState::MRight))
	{
		m_anim->play("walk_right",dt, std::abs(m_movement->getVelocity().x / m_movement->getMaxVelocity()) );
		m_dir = CharDir::Right;
	}

	else if (m_movement->getState(MovingState::MUp))
	{
		m_anim->play("walk_up",dt, std::abs(m_movement->getVelocity().y / m_movement->getMaxVelocity()) );
		m_dir = CharDir::Up;
	}

	else if (m_movement->getState(MovingState::MDown))
	{
		m_anim->play("walk_down",dt, std::abs(m_movement->getVelocity().y / m_movement->getMaxVelocity()) );
		m_dir = CharDir::Down;
	}	
}
//-Core
void Player::update(float dt)
{
	m_movement->update();
	m_movement->apply(m_sprite, dt);

	updateAnims(dt);
}
