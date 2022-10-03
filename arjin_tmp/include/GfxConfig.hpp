#ifndef GFX_CONFIG_HPP
#define GFX_CONFIG_HPP
#include <SFML/Graphics.hpp>
struct GfxConfig
{
	bool fullscreen;
	bool verticalSync;
	int  videoModeIdx;
	unsigned int framerateLimit;
	unsigned int antialiasingLevel;
	std::vector<sf::VideoMode> videoModes;	

	GfxConfig()
		: fullscreen(0),   verticalSync(0)
		, videoModeIdx(0), framerateLimit(0)
		, antialiasingLevel(0)
		, videoModes(sf::VideoMode::getFullscreenModes())
	{

	}
};
#endif