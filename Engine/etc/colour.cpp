/*
 * colour.cpp
 *
 *  Created on: 9/10/2013
 *      Author: drb
 */

#include "colour.h"

namespace etc
{

	SDL_Color toColour ( Colour colour )
	{
		return toColour( colour, 255 );
	}

	SDL_Color toColour ( Colour colour, Uint8 a )
	{
		SDL_Color out;
		out.a = a;
		switch ( colour )
		{
			case( COLOUR_BLACK ):
				out.r = 0;
				out.b = 0;
				out.g = 0;
				break;
			case ( COLOUR_WHITE ):
				out.r = 255;
				out.b = 255;
				out.g = 255;
				break;
			case ( COLOUR_BLUE ):
				out.r = 0;
				out.b = 255;
				out.g = 0;
				break;
			case ( COLOUR_GREEN ):
				out.r = 0;
				out.b = 0;
				out.g = 255;
				break;
			case ( COLOUR_RED ):
				out.r = 255;
				out.b = 0;
				out.g = 0;
				break;

		}
		return out;
	}

	SDL_Color toColour ( int r , int g , int b , int a )
	{
		return toColour( ( Uint8 )r , ( Uint8 )g, ( Uint8 )b, ( Uint8 )a );
	}

	SDL_Color toColour ( Uint8 r , Uint8 g , Uint8 b , Uint8 a )
	{
		SDL_Color out;
		out.r = r;
		out.g = g;
		out.b = b;
		out.a = a;
		return out;

	}

} /* namespace etc */
