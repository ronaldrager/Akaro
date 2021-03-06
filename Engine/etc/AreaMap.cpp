/*
 * AreaMap.cpp
 *
 *  Created on: 20/09/2013
 *      Author: drb
 */

#include "AreaMap.h"
#include <limits>
#include "../maths/rect.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace etc;

#define AREA_MAP_MAX_DEPTH 16
#define AREA_MAP_SPRITE_SPLIT_SIZE 10
//#define AREA_MAP_LIMIT_DEPTH
AreaMap::AreaMap()
{
	// TODO Auto-generated constructor stub
	this->parent = nullptr;
	//Most even spread of all area over the axis
	this->area.x = std::numeric_limits<int>::min() / 2;
	this->area.y = std::numeric_limits<int>::min() / 2;
	//Maximum amount of area possible
	this->area.h = std::numeric_limits<int>::max();
	this->area.w = std::numeric_limits<int>::max();

	children.reserve( 4 );
	sprites.reserve ( 10 );
	this->depth = 0;
}

AreaMap::AreaMap( SDL_Rect Area )
{
	// TODO Auto-generated constructor stub
	this->parent = nullptr;
	//Most even spread of all area over the axis
	this->area = Area;

	children.reserve( 4 );
	sprites.reserve ( 10 );
	this->depth = 0;
}


AreaMap::~AreaMap()
{
	// TODO Auto-generated destructor stub
	this->clear();
}

AreaMap::AreaMap( AreaMap* Parent , SDL_Rect Area )
{
	this->parent = Parent;
	this->area = Area;
	this->depth = Parent->getDepth() + 1;
}

void AreaMap::insert( graphics::drawable* sp )
{
	if ( this->sprites.size() >= AREA_MAP_SPRITE_SPLIT_SIZE || this->children.size() == 4 )
	{
		//Haven't split yet, might as well
		if ( this->children.size() != 4 )
		{
			this->split();
		}
		//Go through children to see if they want it
		for ( unsigned int n = 0; n < this->children.size(); n++ )
		{
			//Check if the sprite belongs to this child's area
			if ( maths::isWholeRectInside( sp->getRect() , this->children[n].getArea() ) )
			{
				this->children[n].insert( sp );
				return;
			}
		}
	}
	//Gives up on passing onto the children and keeps it
	this->sprites.push_back( sp );
}

void AreaMap::getSpritesFromArea ( SDL_Rect Area , std::vector<graphics::drawable*>* out )
{
	for ( unsigned int i = 0; i < this->sprites.size(); i++ )
	{
		if ( maths::isRectTouching( this->sprites[i]->getRect() , Area ) )
		{
			out->push_back( this->sprites[i] );
		}
	}
	for ( unsigned int n = 0; n < this->children.size(); n++ )
	{
		//Make sure the child has a chance of having the area
		if ( maths::isRectTouching( this->children[n].getArea() , Area ) )
		{
			this->children[n].getSpritesFromArea( Area , out );
		}
	}
}

SDL_Rect AreaMap::getArea()
{
	return this->area;
}



void AreaMap::split()
{
#ifdef AREA_MAP_LIMIT_DEPTH
	if ( this->depth >= AREA_MAP_MAX_DEPTH )
	{
		return;
	}
#endif
	//If there are no children, time to make some
	if ( children.size() == 0 )
	{
		SDL_Rect new_area;
		//Width and height will remain const
		new_area.w = this->area.w / 2;
		new_area.h = this->area.h / 2;
		//Position of the rect will change to the four sections

		//Top left
		new_area.x = this->area.x;
		new_area.y = this->area.y;
		this->children.push_back( AreaMap( this ,  new_area ) );

		//Top right
		new_area.x = this->area.x + new_area.w;
		new_area.y = this->area.y;
		this->children.push_back( AreaMap( this ,  new_area ) );

		//bottom left
		new_area.x = this->area.x ;
		new_area.y = this->area.y + new_area.h;
		this->children.push_back( AreaMap( this ,  new_area ) );

		//bottom right
		new_area.x = this->area.x + new_area.w;
		new_area.y = this->area.y + new_area.h;
		this->children.push_back( AreaMap( this ,  new_area ) );
	}

	std::vector<graphics::drawable*> leftovers;
	for ( unsigned int i = 0; i < this->sprites.size(); i++ )
	{
		//Keep track of sprites that are not giving to children
		bool taken = false;
		for ( unsigned int n = 0; n < this->children.size(); n++ )
		{
			//Check if the sprite belongs to this child's area
			if ( maths::isWholeRectInside( sprites[i]->getRect() , this->children[n].getArea() ) )
			{
				this->children[n].insert( sprites[i] );
				taken = true;
			}
		}
		if ( !taken )
		{
			leftovers.push_back( sprites[i] );
		}
	}
	this->sprites = leftovers;
}

void AreaMap::clear()
{
	//Clear sprites in container
	this->sprites.clear();
	//Tell the children to get rid of their toys
	for ( unsigned int n = 0; n < this->children.size(); n++ )
	{
		this->children[n].clear();
	}
	//Take the children out the back... sorry Betsy
	this->children.clear();
}

bool AreaMap::remove ( graphics::drawable* sp )
{
	bool result = false;
	for ( unsigned int i = 0; i < this->sprites.size(); i++ )
	{
		if ( this->sprites[i] == sp )
		{
			this->sprites.erase( this->sprites.begin() + i );
			result = true;
			//Check if the node has children
			if ( this->children.size() > 0 && this->parent != nullptr )
			{
				//Count if the children have objects
				int count = 0;
				for ( unsigned int n = 0; n < this->children.size(); n++ )
				{
					count += this->children[n].count();
					//Shortcut out
					if ( count > 0 )
					{
						break;
					}
				}
				//If there are no object then <strike>kill</strike> delete the unneeded children
				if ( count == 0 )
				{
					this->children.clear();
				}
			}

		}
	}
	for ( unsigned int n = 0; n < this->children.size(); n++ )
	{
		//Check if the sprite belongs to this child's area
		if ( maths::isWholeRectInside( sp->getRect() , this->children[n].getArea() ) )
		{
			result = this->children[n].remove( sp );
		}
	}
	return result;
}

void AreaMap::update ( graphics::drawable* sp )
{
	//Remove the sprite pointer
	this->remove( sp );
	//Insert it again
	this->insert( sp );
}

int AreaMap::count()
{
	//Get own sprite count
	int count = this->sprites.size();
	//Ask Children if they have any
	for ( unsigned int n = 0; n < this->children.size(); n++ )
	{
		count += this->children[n].count();
	}

	return count;
}

int AreaMap::getDepth()
{
	return this->depth;
}
