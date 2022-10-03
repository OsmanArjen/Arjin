#ifndef GAME_MAIN_HPP
#define GAME_MAIN_HPP
#include "GameState.hpp" 
class GameMain
{
private:
	//-Window related
	sf::RenderWindow* m_window;
	sf::Event m_sfEvent;
	sf::Clock m_sfClock;
	float m_deltaTime;
	//-GFX Config
	GfxConfig m_gfxConfig;
	//-State related
	StateData m_stateData;
	std::stack<State*> m_stateStack;
	//-Initializer functions
	void initGFXConfig();
	void initWindow();
	void initStateData();
	void initStates();
	//--Main Operations
	//-Update
	void update();
	void updateDeltaTime();
	void updateEvents();
	void updateStates();
	//-Render
	void render();
public:
	// Constructor/Destructor
	GameMain();
	~GameMain();
	//-Run
	void run();
	
};
#endif // GAME_MAIN_HPP