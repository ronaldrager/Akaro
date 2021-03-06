#include "content.h"

#include "etc/Timer.h"
#include "etc/env.h"
#include "io/file.h"
#include "GameWindow.h"
#include "etc/string.h"
#include "etc/colour.h"

#include <string>
#include <stdlib.h>
#include <sstream>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <ctime>

Content::Content()
{
	//ctor
	this->settings = IO::Settings( IO::SETTING_LOAD_ON_REQUEST );
	this->inited = false;
}

Content::~Content()
{

}

int Content::init()
{
	if ( IO::fileExists( SETTINGS_PATH ) == false )
	{
		std::cout << "Could not initialize due to the settings file "
				  << SETTINGS_PATH << " does not exist." ;
		return 1;
	}

	if ( IO::fileExists( INPUT_SETTINGS_FILE ) == false )
	{
		std::cout << "Could not initialize due to the settings file "
				  << INPUT_SETTINGS_FILE << " does not exist." ;
		return 2;
	}

	settings.load( SETTINGS_PATH , IO::SETTINGS_DUPLICATES_INGORED );
	std::string ver = "";
	if (settings.get("info" , "version" , &ver))
	{
		std::cout << "Version " << ver << std::endl;
	}
	#ifdef DEBUG
	std::cout << SETTINGS_PATH << " loaded." << std::endl;
	#endif

	//Start SDL and Others

	//Start SDL
	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		std::cout << "An error has occurred in starting SDL2" << std::endl << SDL_GetError() << std::endl;
		std::cerr << SDL_GetError() << std::endl;
		return 3;
	}

	//Start SDL_image
	if ( IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG ) == 0 )
	{
		std::cout << "An error has occurred in starting SDL2_image" << std::endl << SDL_GetError() << std::endl;
		std::cerr << SDL_GetError() << std::endl;
		return 4;
	}

	//Start SDL_mixer
	if ( Mix_Init( MIX_INIT_FLAC | MIX_INIT_OGG ) == 0 )
	{
		std::cout << "An error has occurred in starting SDL2_mixer" << std::endl << SDL_GetError() << std::endl;
		std::cerr << SDL_GetError() << std::endl;
		return 5;
	}

	//Start SDL_ttf
	if ( TTF_Init() == -1 )
	{
		std::cout << "An error has occurred in starting SDL2_ttf" << std::endl << SDL_GetError() << std::endl;
		std::cerr << SDL_GetError() << std::endl;
		return 6;
	}
	#ifdef DEBUG
	std::cout << "All sub systems loaded." << std::endl;
	#endif
	inited = true;
	return 0;
}

void Content::load()
{
	if (inited)
		this->audio.load_settings( &(this->settings) );
}

void Content::unload()
{
	if(!inited)
		return;

	this->maps.unloadAll();
	this->fonts.unloadAll();
	this->audio.unloadall();
	this->gamestate.unload();
	this->input.unload();
	this->settings.unload();
	this->tiletypes.unloadAll();

	//Shutdown inited services in reverse order of init
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

	inited= false;
}
/**
 * @brief Returns a pointer to the Window Settings
 */
IO::Settings* Content::Settings ()
{
	return &( this->settings );
}

input::InputManager* Content::Input()
{
	return &( this->input );
}

audio::AudioManager* Content::Audio()
{
	return &( this->audio );
}

GameStateManager* Content::Gamestate()
{
	return &( this->gamestate );
}

ui::FontManager* Content::Fonts()
{
	return &( this->fonts );
}

map::MapManager* Content::Maps()
{
	return &(this->maps);
}

map::TileTypeManager* Content::TileTypes()
{
	return &(this->tiletypes);
}
