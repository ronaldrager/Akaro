/*
 * drawable.cpp
 *
 *  Created on: 15/10/2013
 *      Author: drb
 */

#include "drawable.h"

namespace graphics
{

  drawable::drawable()
  {
    //Set the rectangle to be (0,0,0,0)
    this->pos.x = 0;
    this->pos.y = 0;

    this->area.x = 0;
    this->area.y = 0;
    this->area.w = 0;
    this->area.h = 0;

  }

  void drawable::setPosition (SDL_Point pos)
  {
    this->pos.x = pos.x;
    this->pos.y = pos.y;

    this->area.x = pos.x;
    this->area.y = pos.y;
  }

  void drawable::setPosition (maths::Point pos)
  {
    this->pos.x = pos.x;
    this->pos.y = pos.y;

    this->area.x = (int)pos.x;
    this->area.y = (int)pos.y;
  }

  void drawable::setPosition (double x , double y)
  {
    this->pos.x = x;
    this->pos.y = y;

    this->area.x = (int)pos.x;
    this->area.y = (int)pos.y;
  }

  SDL_Rect drawable::getRect ()
  {
    return this->area;
  }

  maths::Point drawable::getPosition ()
  {
    return this->pos;
  }

  drawable::~drawable()
  {
  }

} /* namespace graphics */
