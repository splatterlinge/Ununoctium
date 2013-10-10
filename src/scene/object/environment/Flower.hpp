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

#ifndef SCENE_OBJECT_ENVIRONMENT_FLOWER_INCLUDED
#define SCENE_OBJECT_ENVIRONMENT_FLOWER_INCLUDED

#include "AVegetation.hpp"
#include "../../Scene.hpp"

#include <QPointF>
#include <QVector>
#include <QMatrix4x4>


class Landscape;
class StaticModel;

class Flower : public AVegetation
{
public:
	Flower( Landscape * landscape, const QString & filename, const QPoint & mapPosition, int mapRadius, int number, int priority );
	virtual ~Flower();

	virtual void updateSelf( const double & delta );
	virtual void drawSelf();
	QVector<QVector3D> getInstances(){ return mPositions; }

	virtual QVector<const AObject*> collideSphere( const AObject * exclude, const float & radius, QVector3D & center, QVector3D * normal );

private:
	Landscape * mLandscape;
	QVector<QMatrix4x4> mInstances;
	QVector<QVector3D> mPositions;
	StaticModel * mModel;
};


#endif
