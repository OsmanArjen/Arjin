#include "GameState.hpp"

// Constructor/Destructor
GameState::GameState(StateData* stateData)
	: State(stateData)
	, m_window(stateData->window)
{
	
}

GameState::~GameState()
{

}

// GameState : Events, Update, Render
void GameState::handleEvents(sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Escape)
		{
			quitState();
	    }
	}
}

void GameState::update(const float& dt)
{

}

void GameState::render()
{
	// Render game state
	//...
}
