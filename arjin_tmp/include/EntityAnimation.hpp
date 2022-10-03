#ifndef ENTITY_ANIM_HPP
#define ENTITY_ANIM_HPP
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <map>
class EntityAnimation
{
private:
	// Inner class for animations
	class Animation;

	sf::Sprite&  m_sprite;
	sf::Texture& m_spriteSheet;
	std::map<std::string, Animation*> m_anims;
	Animation* m_lastAnim;
public:
	EntityAnimation(sf::Sprite& sprite, sf::Texture& spritesheet);
	~EntityAnimation();
	void add(const std::string& key, 
		     sf::Vector2i startFramePos, 
		     sf::Vector2i endFramePos, 
		     sf::Vector2i clipSize,
		     float delayLimit);
	bool play(const std::string& key, float dt, float modPercent = 1.0f);
	void resetLastAnim();
	
};

#endif // ENTITY_ANIM_HPP