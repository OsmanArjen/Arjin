#include "EntityAnimation.hpp"

class EntityAnimation::Animation
{
private:
	// Attributes
	sf::Sprite&  m_sprite;
	sf::Texture& m_spriteSheet;
	sf::IntRect  m_startRect, m_currRect, m_endRect;
	int m_clipWidth, m_clipHeight;
	float m_delayLimit, m_timer;
	bool m_isdone;

public:
	// Constructor
	Animation(sf::Sprite&  sprite,   
	          sf::Texture& spritesheet, 
	          sf::Vector2i startFramePos, 
	          sf::Vector2i endFramePos, 
	          sf::Vector2i clipSize,
	          float delayLimit)
		: m_sprite(sprite)
		, m_spriteSheet(spritesheet)
		, m_startRect(startFramePos.x * clipSize.x, startFramePos.y * clipSize.y, clipSize.x, clipSize.y)
		, m_currRect(m_startRect)
		, m_endRect(endFramePos.x     * clipSize.x, endFramePos.y   * clipSize.y, clipSize.x, clipSize.y)
		, m_clipWidth(clipSize.x)
		, m_clipHeight(clipSize.y)
		, m_delayLimit(delayLimit)
		, m_timer(0.0f)
		, m_isdone(false)
	{
		// Select start frame of animation
		m_sprite.setTexture(m_spriteSheet, true);
		m_sprite.setTextureRect(m_startRect);
	}
	
	// Functions
	bool play(float dt, float modPercent = 1.0f)
	{
		// To avoid magic numbers
		constexpr float delayVal{100.0f};
		constexpr float minModVal{0.5f};

		// Limit the percentage of modify value to avoid too slow animation
		modPercent = (modPercent < minModVal) ? minModVal : modPercent;

		// Update timer
		m_isdone = false;
		m_timer += modPercent * delayVal * dt;
		if (m_timer >= m_delayLimit)
		{
			// Reset the timer
			m_timer = 0.0f;
			// Animate
			if(m_currRect != m_endRect)
			{
				m_currRect.left += m_clipWidth;
			}
			else // Reset
			{
				m_currRect.left = m_startRect.left;
				m_isdone = true;
			}
			
			m_sprite.setTextureRect(m_currRect);
		}
		return m_isdone;
	}

	void reset()
	{
		
		m_timer    = m_delayLimit; // Skipping the first frame to avoiding wait while switching anims
		m_currRect = m_startRect;
	}
};

EntityAnimation::EntityAnimation(sf::Sprite& sprite, sf::Texture& spritesheet)
	: m_sprite(sprite)
	, m_spriteSheet(spritesheet)
	, m_lastAnim(nullptr)
{

}

EntityAnimation::~EntityAnimation()
{
	for(auto& anim : m_anims)
	{
		delete anim.second;
	}
}

void EntityAnimation::add(const std::string& key, 
	                      sf::Vector2i startFramePos, 
	                      sf::Vector2i endFramePos, 
	                      sf::Vector2i clipSize,
	                      float delayLimit)
{
	m_anims[key] = new Animation(m_sprite,      m_spriteSheet, 
	                             startFramePos, endFramePos, 
	                             clipSize,      delayLimit);
}

bool EntityAnimation::play(const std::string& key, float dt, float modPercent)
{
	if(m_lastAnim != m_anims[key])
	{
		if(m_lastAnim != nullptr)
			m_lastAnim->reset();
			
		m_lastAnim = m_anims[key];
	}

	return m_anims[key]->play(dt, modPercent);	
}


void EntityAnimation::resetLastAnim()
{
	if(m_lastAnim != nullptr)
		m_lastAnim->reset();
}