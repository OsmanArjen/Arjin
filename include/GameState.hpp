#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP
#include "State.hpp"
#include "EntityManager.hpp"
class GameState: public State
{
private:
	sf::RenderWindow* m_window;
	
public:
	// Constructor/Destructor
	GameState(StateData* stateData);
	~GameState();

	// GameState : Events, Update, Render
	void handleEvents(sf::Event& event);
	void update(const float& dt);
	void render();
	
};
#endif