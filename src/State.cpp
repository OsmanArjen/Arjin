#include "State.hpp"

State::State(StateData* stateData)
	: m_quit(false)
	, m_stateData(stateData) 
{

}

void State::quitState()
{
	m_quit = true;
}

bool State::getQuitStatus()
{
	return m_quit;
}

// Static Functions
Json::Value State::readJSONConfig(const std::string& path) // For Config Files
{

	std::ifstream file(path, std::ifstream::binary);

	Json::CharReaderBuilder builder;
	Json::Value value;
	std::string errs;
	bool state{Json::parseFromStream(builder, file, &value, &errs)};
	if ( !state ) // Error while parsing JSON file
	{
		std::cerr << errs << std::endl; 
	}
	
	file.close();

	return value;
}