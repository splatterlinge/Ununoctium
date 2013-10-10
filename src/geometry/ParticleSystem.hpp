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

#ifndef GEOMETRY_PARTICLESYSTEM_INCLUDED
#define GEOMETRY_PARTICLESYSTEM_INCLUDED

#include "Vertex.hpp"

#include <QVector>
#include <QVector2D>
#include <QVector3D>


/// Simple particle system
class ParticleSystem
{
public:
	/// A single particle
	class Particle
	{
	public:
		Particle( float life=0.0f, QVector3D position=QVector3D(0,0,0), QVector3D velocity=QVector3D(0,0,0) ) :
			mVelocity(velocity), mPosition(position), mLife(life), mRotation(rand()%4) {}
		~Particle() {}
		const QVector3D & velocity() const { return mVelocity; }
		const QVector3D & position() const { return mPosition; }
		const float & life() const { return mLife; }
		const int & rotation() const { return mRotation; }
		QVector3D & rVelocity() { return mVelocity; }
		QVector3D & rPosition() { return mPosition; }
		float & rLife() { return mLife; }
		void setVelocity( const QVector3D & velocity ) { mVelocity = velocity; }
		void setPosition( const QVector3D & position ) { mPosition = position; }
		void setLife( const float & life ) { mLife = life; }
	private:
		QVector3D mVelocity;
		QVector3D mPosition;
		float mLife;
		int mRotation;
	};

	/// Inherit to define particle interaction with environment
	class Interactable
	{
	public:
		virtual void particleInteraction( const double & delta, Particle & particle ) = 0;
	};

	ParticleSystem( int capacity=1000 );
	~ParticleSystem() {}
	void update( const double & delta );
	void draw( const QMatrix4x4 & modelView );
	void emitSpherical( const QVector3D & source, int toEmit, const float & minVel, const float & maxVel, const QVector3D & velOffset = QVector3D(0,0,0) );
	const float & minLife() const { return mMinLife; }
	const float & maxLife() const { return mMaxLife; }
	const float & drag() const { return mDrag; }
	const float & size() const { return mSize; }
	const QVector3D & gravity() const { return mGravity; }
	const int capacity() const { return mParticles.size(); }
	void setMinLife( const float & minLife ) { mMinLife = minLife; }
	void setMaxLife( const float & maxLife ) { mMaxLife = maxLife; }
	void setDrag( const float & drag ) { mDrag = drag; }
	void setSize( const float & size ) { mSize = size; }
	void setGravity( const QVector3D & gravity ) { mGravity = gravity; }
	void setCapacity( const int & capacity ) { mParticles.resize( capacity ); mParticleVertices.resize( capacity*4 ); }
	void setInteractionCallback( Interactable * callback ) { mInteractionCallback = callback; }

protected:

private:
	float mMinLife;
	float mMaxLife;
	float mDrag;
	float mSize;
	QVector3D mGravity;
	QVector<Particle> mParticles;
	QVector<VertexP3fN3fT2f> mParticleVertices;
	Interactable * mInteractionCallback;
};


#endif
