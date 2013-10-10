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

#include "Laser.hpp"

#include "../World.hpp"


#include <geometry/ParticleSystem.hpp>
#include <resource/AudioSample.hpp>
#include <resource/Material.hpp>
#include <scene/Scene.hpp>
#include <utility/Quaternion.hpp>


Laser::Laser( World * world ) :
	AWeapon( world, 20, 1, 1 )
{
	mQuadric = gluNewQuadric();
	gluQuadricTexture( mQuadric, GL_TRUE );

	mName = "Laser";
	mDrawn = false;
	mTrailVisibilityDuration = 1.0f;
	mHeat = 0.0f;
	mTrailAlpha = 0.0f;
	mFired = false;
	mRange = 250.0f;
	mTrailRadius = 0.04f;
	mDamage = 50.0f;

	mMaterial = new Material( scene()->glWidget(), "KirksEntry" );

	mFireSound = new AudioSample( "laser" );
	mFireSound->setLooping( false );
	mReloadSound = new AudioSample( "laser_reload" );
	mReloadSound->setLooping( false );

	mImpactParticleMaterial = new Material( scene()->glWidget(), "GlowParticle" );
	mImpactParticles = new ParticleSystem( 64 );
	mImpactParticles->setSize( 0.25f );
	mImpactParticles->setGravity( QVector3D( 0.0f, -20.0f, 0.0f ) );
	mImpactParticles->setDrag( 0.75f );
	mImpactParticles->setMinLife( 1.0f );
	mImpactParticles->setMaxLife( 2.0f );
}


Laser::~Laser()
{
	gluDeleteQuadric( mQuadric );
	delete mImpactParticleMaterial;
	delete mImpactParticles;
	delete mMaterial;
	delete mFireSound;
	delete mReloadSound;
}


void Laser::triggerPressed()
{
	mFired = true;
}


void Laser::triggerReleased()
{
	mFired = false;
}


void Laser::pull()
{
	mDrawn = true;
}


void Laser::holster()
{
	mDrawn = false;
}


void Laser::setTarget( const QVector3D * target )
{
	mTarget = target;
}


void Laser::updateSelf( const double & delta )
{
	mImpactParticles->update( delta );
	if( mDrawn )
	{
		setRotation( QQuaternion::slerp( rotation(), getRotationToTarget( mTarget, 0.3f ), 20.0 * delta ) );
		if( mFired )
		{
			if( mHeat <= 0.0f && mClipAmmo > 0 )
			{
				mHeat = 3.0f;
				mTrailAlpha = 1.0f;
				mTrailStart = worldPosition();
				mTrailDirection = worldDirection();
				mTrailLength = mRange;
				AObject * target = const_cast<AObject*>( world()->intersectLine( this, mTrailStart, mTrailDirection, mTrailLength ) );
				mTrailEnd = mTrailStart + mTrailDirection*mTrailLength;

				if( target )
					mImpactParticles->emitSpherical( mTrailEnd, 64, 5.0, 10.0, QVector3D(0,10,0) );
				ACreature * victim = dynamic_cast<ACreature*>(target);
				if( victim )
					victim->receiveDamage( mDamage, &mTrailEnd, &mTrailDirection );

				mFireSound->play();
				mReloadSound->play();
				mClipAmmo--;
				mAmmo--;
			}
		}
	}

	if( mHeat > delta )
	{
		mHeat -= delta;
	}
	else
	{
		mHeat = 0.0f;
		mClipAmmo = mClipSize;
	}

	if( mTrailAlpha > delta )
		mTrailAlpha -= delta / mTrailVisibilityDuration;
	else
		mTrailAlpha = 0.0f;
}


void Laser::drawSelf()
{
	if( !mDrawn )
		return;

	mMaterial->bind();
	glTranslatef( 0.0f, 0.0f, -0.2f );
	gluCylinder( mQuadric, 0.08f, 0.08f, 0.4f, 16, 16 );
	gluCylinder( mQuadric, 0.15f, 0.10f, 0.2f, 16, 16 );
	mMaterial->release();
}


void Laser::draw2Self()
{
	glPushMatrix();
	glLoadMatrix( world()->modelViewMatrix() );

	glDepthMask( GL_FALSE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );

	// particles on impact
	glEnable( GL_TEXTURE_2D );
	glColor4f( 0.2f, 0.4f, 1.0f, 1.0f );
	mImpactParticleMaterial->bind();
	mImpactParticles->draw( world()->modelViewMatrix() );
	mImpactParticleMaterial->release();

	// trail
	glColor4f( 0.2f, 0.4f, 1.0f, mTrailAlpha );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );
	QVector3D toEye = scene()->eye()->position() - mTrailStart;
	QVector3D crossDir = QVector3D::crossProduct( mTrailDirection, toEye ).normalized();
	glBegin( GL_TRIANGLE_STRIP );
		glVertex(-crossDir*mTrailRadius + mTrailStart);
		glVertex( crossDir*mTrailRadius + mTrailStart);
		glVertex(-crossDir*mTrailRadius + mTrailEnd );
		glVertex( crossDir*mTrailRadius + mTrailEnd );
	glEnd();

	glColor4f(1,1,1,1);
	glDisable( GL_BLEND );
	glDepthMask( GL_TRUE );

	glPopMatrix();
}
