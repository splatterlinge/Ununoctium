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

#include "Intersection.hpp"
#include "Triangle.hpp"


bool Intersection::intersectTriangleFan(const GLfloat PositionData[], const int firstVertexPos, const int lastVertexPos,
	const QMatrix4x4 & modelMatrix, const QVector3D & origin, const QVector3D & direction, float * intersectionDistance )
{
	QVector3D v[3];
	v[0] = QVector3D(PositionData[firstVertexPos*3], PositionData[firstVertexPos*3+1], PositionData[firstVertexPos*3+2]);
	v[1] = QVector3D(PositionData[firstVertexPos*3+3], PositionData[firstVertexPos*3+4], PositionData[firstVertexPos*3+5]);

	for(int i = firstVertexPos+2; i <= lastVertexPos; i++)
	{
		if(i != firstVertexPos+2)
		{
			v[1] = v[2];
		}

		v[2] = QVector3D(PositionData[i*3], PositionData[i*3+1], PositionData[i*3+2]);

		if( Triangle::intersectRay(modelMatrix.map(v[0]), modelMatrix.map(v[1]), modelMatrix.map(v[2]), origin, direction, intersectionDistance) ||
			Triangle::intersectRay(modelMatrix.map(v[0]), modelMatrix.map(v[2]), modelMatrix.map(v[1]), origin, direction, intersectionDistance) )
		{
			return true;
		}
	}
	return false;
}



bool Intersection::intersectTriangleStrip(const GLfloat PositionData[], const int firstVertexPos, const int lastVertexPos,
	const QMatrix4x4 & modelMatrix, const QVector3D & origin, const QVector3D & direction, float * intersectionDistance )
{
	QVector3D v[3];
	v[0] = QVector3D(PositionData[firstVertexPos*3], PositionData[firstVertexPos*3+1], PositionData[firstVertexPos*3+2]);
	v[1] = QVector3D(PositionData[firstVertexPos*3+3], PositionData[firstVertexPos*3+4], PositionData[firstVertexPos*3+5]);

	for(int i = firstVertexPos+2; i <= lastVertexPos; i++)
	{
		if(i != firstVertexPos+2){
			v[0] = v[1];
			v[1] = v[2];
		}

		v[2] = QVector3D(PositionData[i*3], PositionData[i*3+1], PositionData[i*3+2]);

		if( Triangle::intersectCulledRay(modelMatrix.map(v[0]), modelMatrix.map(v[1]), modelMatrix.map(v[2]), origin, direction, intersectionDistance) ||
			Triangle::intersectCulledRay(modelMatrix.map(v[1]), modelMatrix.map(v[0]), modelMatrix.map(v[2]), origin, direction, intersectionDistance) )
		{
			return true;
		}
	}
	return false;
}
