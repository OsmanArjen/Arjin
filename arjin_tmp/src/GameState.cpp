#include "GameState.hpp"

// Constructor/Destructor
GameState::GameState(StateData* stateData)
	: State(stateData)
	, m_player({0.0f,0.0f})
	, m_window(stateData->window)
{

}

GameState::~GameState()
{

}

// GameState : Events, Update, Render
void GameState::handleEvents(sf::Event& event)
{

}

void GameState::update(float dt)
{
	// Escape key for fullscreen just for now
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		quitState();
	}

	// Key Controls
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		m_player.move(0.0f, -1.0f);
	}

	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		m_player.move(0.0f,  1.0f);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_player.move(1.0f,  0.0f);
	}
	
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_player.move(-1.0f, 0.0f);
	}

	m_player.update(dt);
}

void GameState::render()
{
	// Render game state
	//...
	m_window->draw(m_player);
}
