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

#ifndef SCENE_OBJECT_CREATURE_DUMMY_INCLUDED
#define SCENE_OBJECT_CREATURE_DUMMY_INCLUDED


#include "ACreature.hpp"


struct GLUquadric;
class Material;


/// Dummy creature
class Dummy : public ACreature
{
public:
	Dummy( World * world );
	~Dummy();

	virtual void updateSelf( const double & delta );
	virtual void drawSelf();

	virtual const AObject * intersectLine( const AObject * exclude, const QVector3D & origin, const QVector3D & direction,
		float & length, QVector3D * normal = NULL ) const;

	virtual void receiveDamage( int damage, const QVector3D * position=NULL, const QVector3D * direction=NULL );
private:
	GLUquadric * mQuadric;
	Material * mMaterial;
	QVector3D mTarget;
	float mVelocityY;
	float mHeightAboveGround;
};


#endif
