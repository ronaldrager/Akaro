/*
 * MenuState.h
 *
 *  Created on: Nov 6, 2013
 *      Author: drb
 */

#ifndef MENUSTATE_H_
#define MENUSTATE_H_

#include "Engine/states/GameState.h"
#include "Engine/states/GameStateManager.h"
#include "Engine/GameWindow.h"
#include "Engine/graphics/Sprite.h"
#include "Engine/map/Map.h"

class MenuState : public GameState
{
	public:
		MenuState( GameStateManager* Manager, GameWindow* gamewindow , Content* content );
		virtual
		~MenuState();

		//Each frame
		void render ( const Ldouble& delta, SDL_Renderer* renderer , etc::Camera& camera );
		void update ( const Ldouble& delta );
		void event ( SDL_Event e , const Ldouble& delta );

		//Inital un/Load Assests
		void load   ();
		void unload ();

	private:
		GameStateManager* manager;
		Content* content;
		GameWindow* window;

		map::Map* cur_map;
};

#endif /* MENUSTATE_H_ */
