#ifndef ENTITY_MOV_HPP
#define ENTITY_MOV_HPP
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
// Enum class for move states
enum class MovingState
{
	MLeft,
	MRight,
	MUp,
	MDown,
	Moving,
	Idle,
};

class EntityMovement
{
private:
	//-Core
	void updateVelX();
	void updateVelY();

	// Movement attributes
	float m_maxVel;
	float m_accVal;
	float m_decVal;

	sf::Vector2f m_vel;
public:
	// Constructor/Destructor
	EntityMovement(float maxVel, float accVal, float decVal);
	~EntityMovement() = default;

	//--Functions
	//-Accessors
	bool getState(MovingState state);
	const sf::Vector2f& getVelocity()    const;
	const float&        getMaxVelocity() const;
	
	//-Modifiers
	void setVelocity(float x, float y);
	void move(float dirx, float diry); // TODO:add dt because acceleration is also function of time

	//-Core
	void update();
	void apply(sf::Sprite& target, float dt); //  TODO:couple this function with update function
	
};
#endif // ENTITY_MOV_HPP