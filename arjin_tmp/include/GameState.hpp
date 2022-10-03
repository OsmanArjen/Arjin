#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP
#include "State.hpp"
#include "Player.hpp"

class GameState: public State
{
private:
	Player m_player;
	sf::RenderWindow* m_window; // pointer to window in stateData
public:
	// Constructor/Destructor
	GameState(StateData* stateData);
	~GameState();

	// GameState : Events, Update, Render
	void handleEvents(sf::Event& event);
	void update(float dt);
	void render();
	
};
#endif