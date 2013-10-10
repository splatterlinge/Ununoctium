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

#ifndef SCENE_OBJECT_TORCH_INCLUDED
#define SCENE_OBJECT_TORCH_INCLUDED

#include "AWorldObject.hpp"
#include "ALightSource.hpp"

#include <GLWidget.hpp>
#include <utility/OcclusionTest.hpp>
#include <utility/Bilboard.hpp>

#include <QGLBuffer>


class StaticModel;
class Material;


/// A torch
/**
 *
 */
class Torch : public AWorldObject, public ALightSource
{
public:
	Torch( World * world );
	virtual ~Torch();

	virtual void updateSelf( const double & delta );
	virtual void drawSelf();
	virtual void draw2Self();

	virtual void updateLightSource( GLenum light );

	const QVector4D & color() const { return mColor; }

private:
	static const GLfloat sQuadVertices[];
	static QGLBuffer sQuadVertexBuffer;

	OcclusionTest mOcclusionTest;
	Material * mMaterial;
	float mFlareSize;
	float mColorCycle;
	QVector4D mColor;
	float mFlareRotation;
	StaticModel * mModel;
	QVector3D mFlarePosition;
};


#endif
