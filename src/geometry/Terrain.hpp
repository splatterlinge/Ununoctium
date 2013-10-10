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

#ifndef GEOMETRY_TERRAIN_INCLUDED
#define GEOMETRY_TERRAIN_INCLUDED

#include "Vertex.hpp"

#include <GLWidget.hpp>
#include <utility/Triangle.hpp>

#include <QString>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QSize>
#include <QSizeF>
#include <QVector>
#include <QVector3D>
#include <QGLBuffer>

#include <math.h>


/// Generates and draws a mesh based on a heightmap.
/**
 * A terrain is a grid of vertices that lies within the X/Z plane.\n
 * The vertice's height is read from a heightmap.\n
 * The heightmap's resolution also defines the grid's resolution and can be of any size.\n
 */
class Terrain
{
public:
	/// Creates a new terrain from a heightmap.
	/**
	 * @param heightMapPath The path to an image file used as heightmap. This should be a monochrome image.
	 * @param size The volume occupied by this terrain.
	 * @param offset Where to put the origin of the terrain.
	 */
	Terrain( const QString & heightMapPath, const QVector3D & size = QVector3D(1,1,1), const QVector3D & offset = QVector3D(0,0,0), const int & smoothingPasses = 1 );

	/// Frees terrain data
	~Terrain();

	/// Draws the complete terrain.
	/**
	 * The complete terrain is rendered using VBOs.
	 */
	void draw();

	/// Draws the terrain within a rectangle in world coordinates.
	/**
	 * The terrain is rendered using VBOs.
	 * @param x Mnimum X-coordinate of the rectangle in world coordinates.
	 * @param z Mnimum Z-coordinate of the rectangle in world coordinates.
	 * @param rangeX Width of the rectangle in X-direction using world coordinates.
	 * @param rangeZ Width of the rectangle in Z-direction using world coordinates.
	 */
	void drawPatch( float x, float z, float rangeX, float rangeZ )
		{ drawPatchMap( toMap( QRectF( x, z, rangeX, rangeZ ) ) ); }

	/// Draws the terrain within a rectangle in world coordinates.
	/**
	 * The terrain is rendered using VBOs.
	 * @param rect The rectangle to draw in world coordinates.
	 */
	void drawPatch( const QRectF & rect )
		{ drawPatchMap( toMap(rect) ); }

	/// Draws the terrain within a rectangle in heightmap coordinates.
	/**
	 * The terrain is rendered using VBOs.
	 * @param x Mnimum X-coordinate of the rectangle in heightmap coordinates.
	 * @param y Mnimum Y-coordinate of the rectangle in heightmap coordinates.
	 * @param width Width of the rectangle in heightmap coordinates.
	 * @param height Height of the rectangle in heightmap coordinates.
	 */
	void drawPatchMap( int x, int y, int width, int height )
		{ drawPatchMap( QRect( x, y, width, height ) ); }

	/// Draws the terrain within a rectangle in heightmap coordinates.
	/**
	 * The terrain is rendered using VBOs.
	 * @param rect The rectangle to draw in heightmap coordinates.
	 */
	void drawPatchMap( const QRect & rect );

	const QSizeF & toMapFactor() const { return mToMapFactor; }

	QPointF toMapF( const QVector3D & point ) const;	///< Converts a vector in world coordinates to heightmap coordinates.
	QPointF toMapF( const QPointF & point ) const;		///< Converts a point in world coordinates to heightmap coordinates.
	QSizeF toMapF( const QSizeF & size ) const;		///< Converts a size in world coordinates to heightmap coordinates.
	QRectF toMapF( const QRectF & rect ) const;		///< Converts a point in world coordinates to heightmap coordinates.
	QPoint toMap( const QVector3D & point ) const;		///< Converts a vector in world coordinates to heightmap coordinates.
	QPoint toMap( const QPointF & point ) const;		///< Converts a point in world coordinates to heightmap coordinates.
	QSize toMap( const QSizeF & size ) const;		///< Converts a size in world coordinates to heightmap coordinates.
	QRect toMap( const QRectF & rect ) const;		///< Converts a rectangle in world coordinates to heightmap coordinates.

	QPointF fromMapF( const QPointF & point ) const;	///< Converts a point in heightmap coordinates to world coordinates.
	QSizeF fromMapF( const QSizeF & size ) const;		///< Converts a size in heightmap coordinates to world coordinates.
	QPointF fromMap( const QPoint & point ) const;		///< Converts a point in heightmap coordinates to world coordinates.
	QSizeF fromMap( const QSize & size ) const;		///< Converts a size in heightmap coordinates to world coordinates.

	const QSize & mapSize() const { return mMapSize; }	///< The size of the heightmap.
	const QVector3D & size() const { return mSize; }	///< The size of the terrain.
	const QVector3D & offset() const { return mOffset; }	///< The offset of the terrain.

	const QVector3D & getVertexPosition( const int & x, const int & y ) const;	///< The vertex at heightmap coordinates.
	const QVector3D & getVertexPosition( const QPoint & p ) const;			///< The vertex at heightmap coordinates.
	const QVector3D & getVertexNormal( const int & x, const int & y ) const;	///< The normal at heightmap coordinates.
	const QVector3D & getVertexNormal( const QPoint & p ) const;			///< The normal at heightmap coordinates.

	/// Returns the rotation needed to match the terrains surface normal
	QQuaternion getNormalRotation( const QVector3D & position, const QVector3D & from = QVector3D(0,1,0) ) const;
	/// Returns the rotation needed to match the terrains surface normal
	QQuaternion getNormalRotation( const QPointF & position, const QVector3D & from = QVector3D(0,1,0) ) const;

	bool getTriangle( const QVector3D & position, Triangle & t ) const;	///< Returns the triangle below a position if existing
	bool getTriangle( const QPointF & position, Triangle & t ) const;	///< Returns the triangle below a position if existing
	Triangle getTriangle( const QVector3D & position ) const;		///< Returns the nearest triangle below a position
	Triangle getTriangle( const QPointF & position ) const;			///< Returns the nearest triangle below a position
	bool getNormal( const QVector3D & position, QVector3D & normal ) const;	///< Returns the normal below a position if existing
	bool getNormal( const QPointF & position, QVector3D & normal ) const;	///< Returns the normal below a position if existing
	QVector3D getNormal( const QVector3D & position ) const;		///< Returns the nearest normal below a position
	QVector3D getNormal( const QPointF & position ) const;			///< Returns the nearest normal below a position
	bool getHeight( const QVector3D & position, float & height ) const;	///< Returns the height of the terrain below a position if existing
	bool getHeight( const QPointF & position, float & height ) const;	///< Returns the height of the terrain below a position if existing
	float getHeight( const QVector3D & position ) const;			///< Returns the height of the terrain below a position
	float getHeight( const QPointF & position ) const;			///< Returns the height of the terrain below a position
	bool getHeightAboveGround( const QVector3D & position, float & heightAboveGround ) const;	///< Returns the height above terrain if existing
	float getHeightAboveGround( const QVector3D & position ) const;					///< Returns the height above terrain

	/// Calculates the intersection distance to the terrain. length is used as input and output.
	bool intersectLine( const QVector3D & origin, const QVector3D & direction, float & length, QVector3D * normal ) const;

protected:

private:
	VertexP3fN3fT2f & vertex( const int & x, const int & y );
	VertexP3fN3fT2f & vertex( const QPoint & p );

	QVector<QVector3D> smoothedPositions( const QVector< QVector3D > & in, const QSize & size );

	bool getLineQuadIntersection( const QVector3D & origin, const QVector3D & direction, const QPoint & quadMapCoord, float & length ) const;

	QSize mMapSize;
	QVector3D mOffset;
	QVector3D mSize;
	QVector<VertexP3fN3fT2f> mVertices;
	QGLBuffer mIndexBuffer;
	QGLBuffer mVertexBuffer;
	QSizeF mToMapFactor;
};


inline QPointF Terrain::toMapF( const QVector3D & point ) const
{
	return QPointF(
		(point.x()-(float)mOffset.x()) * mToMapFactor.width(),
		(point.z()-(float)mOffset.z()) * mToMapFactor.height()
	);
}


inline QPointF Terrain::toMapF( const QPointF & point ) const
{
	return QPointF(
		(point.x()-(float)mOffset.x()) * mToMapFactor.width(),
		(point.y()-(float)mOffset.z()) * mToMapFactor.height()
	);
}


inline QSizeF Terrain::toMapF( const QSizeF & size ) const
{
	return QSizeF(
		size.width() * mToMapFactor.width(),
		size.height() * mToMapFactor.height()
	);
}


inline QRectF Terrain::toMapF( const QRectF & rect ) const
{
	return QRectF(
		toMapF(rect.topLeft()),
		toMapF(rect.size())
	);
}


inline QPoint Terrain::toMap( const QVector3D & point ) const
{
	return QPoint(
		floorf( (point.x()-mOffset.x()) * mToMapFactor.width() ),
		floorf( (point.z()-mOffset.z()) * mToMapFactor.height() )
	);
}


inline QPoint Terrain::toMap( const QPointF & point ) const
{
	return QPoint(
		floorf( (point.x()-mOffset.x()) * mToMapFactor.width() ),
		floorf( (point.y()-mOffset.z()) * mToMapFactor.height() )
	);
}


inline QSize Terrain::toMap( const QSizeF & size ) const
{
	return QSize(
		ceilf( size.width() * mToMapFactor.width() ),
		ceilf( size.height() * mToMapFactor.height() )
	);
}


inline QRect Terrain::toMap( const QRectF & rect ) const
{
	return QRect(
		toMap(rect.topLeft()),
		toMap(rect.size())
	);
}


inline QPointF Terrain::fromMapF( const QPointF & point ) const
{
	return QPointF(
		point.x()/mToMapFactor.width() + (float)mOffset.x(),
		point.y()/mToMapFactor.height() + (float)mOffset.z()
	);
}


inline QSizeF Terrain::fromMapF( const QSizeF & size ) const
{
	return QSizeF(
		size.width() / mToMapFactor.width(),
		size.height() / mToMapFactor.height()
	);
}


inline QPointF Terrain::fromMap( const QPoint & point ) const
{
	return QPointF(
		point.x()/mToMapFactor.width() + (float)mOffset.x(),
		point.y()/mToMapFactor.height() + (float)mOffset.z()
	);
}


inline QSizeF Terrain::fromMap( const QSize & size ) const
{
	return QSize(
		size.width() / mToMapFactor.width(),
		size.height() / mToMapFactor.height()
	);
}


inline bool Terrain::getTriangle( const QVector3D & position, Triangle & t ) const
{
	return getTriangle( QPointF(position.x(),position.z()), t );
}


inline Triangle Terrain::getTriangle( const QVector3D & position ) const
{
	return getTriangle( QPointF(position.x(),position.z()) );
}


inline bool Terrain::getNormal( const QVector3D & position, QVector3D & normal ) const
{
	return getNormal( QPointF(position.x(),position.z()), normal );
}


inline QVector3D Terrain::getNormal( const QVector3D & position ) const
{
	return getNormal( QPointF(position.x(),position.z()) );
}

inline QQuaternion Terrain::getNormalRotation( const QVector3D & position, const QVector3D & from ) const
{
	return getNormalRotation( QPointF( position.x(), position.z() ), from );
}


inline bool Terrain::getHeight( const QVector3D & position, float & height ) const
{
	return getHeight( QPointF(position.x(),position.z()), height );
}


inline float Terrain::getHeight( const QVector3D & position ) const
{
	return getHeight( QPointF(position.x(),position.z()) );
}


inline bool Terrain::getHeightAboveGround( const QVector3D & position, float & heightAboveGround ) const
{
	bool ret = getHeight( QPointF(position.x(),position.z()), heightAboveGround );
	if( ret )
		heightAboveGround = position.y() - heightAboveGround;
	return ret;
}


inline float Terrain::getHeightAboveGround( const QVector3D & position ) const
{
	return position.y() - getHeight( QPointF(position.x(),position.z()) );
}


inline const QVector3D & Terrain::getVertexPosition( const int & x, const int & y ) const
{
	return mVertices[x+y*mMapSize.width()].position;
}


inline const QVector3D & Terrain::getVertexPosition( const QPoint & p ) const
{
	return getVertexPosition( p.x(), p.y() );
}


inline const QVector3D & Terrain::getVertexNormal( const int & x, const int & y ) const
{
	return mVertices[x+y*mMapSize.width()].normal;
}


inline const QVector3D & Terrain::getVertexNormal( const QPoint & p ) const
{
	return getVertexNormal( p.x(), p.y() );
}


inline VertexP3fN3fT2f & Terrain::vertex( const int & x, const int & y )
{
	return mVertices[x+y*mMapSize.width()];
}


inline VertexP3fN3fT2f & Terrain::vertex( const QPoint & p )
{
	return vertex( p.x(), p.y() );
}


#endif
