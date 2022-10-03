#include "GameMain.hpp"

// Initializer functions
void GameMain::initGFXConfig()
{
	Json::Value gfxCfg{State::readJSONConfig("../config/graphics.json")};
	m_gfxConfig.fullscreen        = gfxCfg["fullscreen"].asBool();
	m_gfxConfig.verticalSync      = gfxCfg["vertical_sync"].asBool();
	m_gfxConfig.videoModeIdx      = gfxCfg["video_mode_idx"].asInt();
	m_gfxConfig.framerateLimit    = gfxCfg["framerate_limit"].asInt();
	m_gfxConfig.antialiasingLevel = gfxCfg["antialiasing_level"].asInt();
}

void GameMain::initWindow()
{
	// Context settings for antialising level
	sf::ContextSettings context;
	context.antialiasingLevel = m_gfxConfig.antialiasingLevel;

	// Constants for solution of magic numbers
	const std::string title{"Arjin: Fire of Life"};
	const sf::VideoMode videoMode{m_gfxConfig.videoModes[m_gfxConfig.videoModeIdx]};

	// Initalize the window
	if(m_gfxConfig.fullscreen)
		m_window = new sf::RenderWindow(videoMode, 
		                                title,
		                                sf::Style::Fullscreen,
		                                context);
	else
		m_window = new sf::RenderWindow(videoMode, 
		                                title,
		                                sf::Style::Titlebar | sf::Style::Close, 
		                                context);
	// Window settings
	m_window->setFramerateLimit(m_gfxConfig.framerateLimit);
	m_window->setVerticalSyncEnabled(m_gfxConfig.verticalSync);
}

void GameMain::initStateData()
{
	m_stateData.window     = m_window;
	m_stateData.stateStack = &m_stateStack;
	m_stateData.gfxConfig  = &m_gfxConfig;
}

void GameMain::initStates()
{
	m_stateStack.push(new GameState(&m_stateData));
}

// Constructor/Destructor
GameMain::GameMain()
{
	// Initialize the main game
	initGFXConfig();
	initWindow();
	initStateData();
	initStates();
}

GameMain::~GameMain()
{
	delete m_window;
	while (!m_stateStack.empty())
	{
		delete m_stateStack.top();
		m_stateStack.pop();
	}
}

// Main Operations
void GameMain::update()
{
	// Update deltatime
	updateDeltaTime();
	// Update SFML Events
	updateSFMLEvents();
	// Update states
	updateStates();


}

void GameMain::updateDeltaTime()
{
	/* Update deltatime for fixing movements etc... */
	m_deltaTime = m_sfClock.restart().asSeconds();
}

void GameMain::updateSFMLEvents()
{
	while (m_window->pollEvent(m_sfEvent))
	{
		if(m_sfEvent.type == sf::Event::Closed)
		{
			m_window->close();
		}

		// Handle state events
		m_stateStack.top()->handleEvents(m_sfEvent);
	}
}

void GameMain::updateStates()
{
	if(!m_stateStack.empty())
	{
		if(m_window->hasFocus())
		{
			m_stateStack.top()->update(m_deltaTime);	
			if(m_stateStack.top()->getQuitStatus())
			{
				m_stateStack.top()->quitState();
				delete m_stateStack.top();
				m_stateStack.pop();
			}
		}
	}
	else
	{
		// End application
		m_window->close();
	}
}


void GameMain::render()
{
	m_window->clear(sf::Color::Black);

	// Render game current state
	if (!m_stateStack.empty())
		m_stateStack.top()->render();

	m_window->display();
}

void GameMain::run()
{
	while(m_window->isOpen())
	{
		// Update & Render
		update();
		render();
	}
}
