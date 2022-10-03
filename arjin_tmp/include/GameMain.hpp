#ifndef GAME_MAIN_HPP
#define GAME_MAIN_HPP
#include "GameState.hpp" 
class GameMain
{
private:
	sf::RenderWindow* m_window;
	sf::Event m_sfEvent;
	sf::Clock m_sfClock;
	float m_deltaTime;
	// GFX Config
	GfxConfig m_gfxConfig;
	// State related
	StateData m_stateData;
	std::stack<State*> m_stateStack;
	// Initializer functions
	void initGFXConfig();
	void initWindow();
	void initStateData();
	void initStates();
public:
	// Constructor/Destructor
	GameMain();
	~GameMain();

	//--Main Operations
	//-Update
	void update();
	void updateDeltaTime();
	void updateSFMLEvents();
	void updateStates();
	//-Render
	void render();
	//-Run
	void run();
	
};
#endif // GAME_MAIN_HPP