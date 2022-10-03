#ifndef STATE_HPP
#define STATE_HPP
#include <iostream>
#include <fstream>
// SFML CORE
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
// Stack data structure for state stack
#include <stack>
// JSON Lib
#include <json/json.h>
// Others...
#include "GfxConfig.hpp"

class State;
struct StateData
{
	sf::RenderWindow* window;
	std::stack<State*>* stateStack;
	GfxConfig* gfxConfig;
};

class State
{
private:
	bool m_quit;
protected:
	StateData* m_stateData;
public:
	State(StateData* stateData);
	virtual ~State() = default;
	void quitState();
	bool getQuitStatus();

	// State core functions
	virtual void handleEvents(sf::Event& event) = 0;
	virtual void update(float dt) = 0;
	virtual void render() = 0;


	// Static funcitons
	static Json::Value readJSONConfig(const std::string& path);
}; 
#endif