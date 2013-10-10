/*
 * Copyright (C) 2013
 * Branimir Djordjevic <branimir.djordjevic@gmail.com>
 * Tobias Himmer <provisorisch@online.de>
 * Michael Wydler <michael.wydler@gmail.com>
 * Karl-Heinz Zimmermann <karlzimmermann3787@gmail.com>
 *
 * This file is part of Ununoctium.
 *
 * Ununoctium is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Ununoctium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ununoctium. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SCENE_OBJECT_AWORLDOBJECT_INCLUDED
#define SCENE_OBJECT_AWORLDOBJECT_INCLUDED


#include "AObject.hpp"


class World;


/// Abstract baseclass for an object belonging to a world
/**
 *
 */
class AWorldObject : public AObject
{
public:
	/// Creates a weapon for the given world
	AWorldObject( World * world, float boundingSphereRadius=0.0f );
	/// Frees this object
	virtual ~AWorldObject();

	World * world() { return mWorld; }

private:
	World * mWorld;
};


#endif
