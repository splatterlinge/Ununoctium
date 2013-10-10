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

#ifndef UTILITY_OCCLUSIONTEST_INCLUDED
#define UTILITY_OCCLUSIONTEST_INCLUDED

#include "glWrappers.hpp"

#include <QVector4D>
#include <QGLBuffer>


/// Occlusion testing using hardware accelerated occlusion queries
class OcclusionTest
{
public:
	OcclusionTest();
	~OcclusionTest();

	bool pointVisible( const QVector3D & point=QVector3D(0,0,0) );
	unsigned char randomPointsInUnitSphereVisible( const unsigned char & numPoints );
	unsigned char randomPointsOnUnitSphereVisible( const unsigned char & numPoints );

private:
	GLuint mQuery;
	static QGLBuffer sRandomVertexInSphereBuffer;
	static QGLBuffer sRandomVertexOnSphereBuffer;
};


#endif
