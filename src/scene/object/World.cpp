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

#include "World.hpp"

#include <scene/Scene.hpp>
#include <utility/RandomNumber.hpp>
#include <geometry/ParticleSystem.hpp>
#include <effect/SplatterSystem.hpp>
#include "Landscape.hpp"
#include "Teapot.hpp"
#include "Sky.hpp"

#include <QPainter>
#include <QSettings>


SplatterQuality::Type SplatterQuality::sMaximum = SplatterQuality::HIGH;

SplatterQuality::Type SplatterQuality::fromString( const QString & name )
{
	QString up = name.toUpper();
	if( up == "LOW" )
		return LOW;
	if( up == "MEDIUM" )
		return MEDIUM;
	if( up == "HIGH" )
		return HIGH;
	qWarning() << name << QT_TR_NOOP("is not a valid splatter quality type");
	return LOW;
}


QString SplatterQuality::toString( const SplatterQuality::Type & quality )
{
	switch( quality )
	{
		case SplatterQuality::HIGH:
			return "High";
		case SplatterQuality::MEDIUM:
			return "Medium";
		case SplatterQuality::LOW:
			return "Low";
	}
	return "Low";
}


World::World( Scene * scene, QString name ) :
	AObject( scene )
{
	QSettings s( "./data/world/"+name+".ini", QSettings::IniFormat );
	s.beginGroup( "World" );
		QString skyName = s.value( "skyName", "earth" ).toString();
		QString landscapeName = s.value( "landscapeName", "earth" ).toString();
	s.endGroup();

	mLevel = 1;
	mLevelTime = 0.0f;
	mLevelDuration = 20.0f;
	mTimeLapse = false;
	mTimeReverse = false;
	mTimeOfDay = 0.0f;

	qsrand( QTime::currentTime().msec() );

	mLandscape = QSharedPointer<Landscape>( new Landscape( this, landscapeName ) );
	add( mLandscape );

	mSky = QSharedPointer<Sky>( new Sky( this, skyName ) );
	add( mSky );

	mPlayer = QSharedPointer<Player>( new Player( this ) );
	add( mPlayer );
	scene->eye()->attach( mPlayer );

	mTeapot = QSharedPointer<Teapot>( new Teapot( scene, 2 ) );
	mTeapot->setPositionY( mLandscape->terrain()->getHeight( QPointF(0,0) ) );
	add( mTeapot );

	addRandomEnemy();

	scene->addKeyListener( this );

	mTarget = QVector3D(0,0,0);

	mSplatterSystem = new SplatterSystem
	(
		scene->glWidget(),
		mLandscape->terrain(),
		"SplatterBig",
		"Splatter",
		"splatter"
	);
	mSplatterInteractor = new SplatterInteractor( *this );
	mSplatterSystem->particleSystem()->setInteractionCallback( mSplatterInteractor );
}


World::~World()
{
	mLightSources.clear();
	scene()->removeKeyListener( this );
	delete mSplatterSystem;
	delete mSplatterInteractor;
}


void World::addLightSource( ALightSource * lightSource )
{
	if( !mLightSources.contains( lightSource ) )
		mLightSources.append( lightSource );
}


void World::removeLightSource( ALightSource * lightSource )
{
	mLightSources.removeOne( lightSource );
}


void World::keyPressEvent( QKeyEvent * event )
{
	switch( event->key() )
	{
	case Qt::Key_PageDown:
		mTimeLapse = true;
		break;
	case Qt::Key_PageUp:
		mTimeReverse = true;
		break;
	default:
		break;
	}
}


void World::keyReleaseEvent( QKeyEvent * event )
{
	switch( event->key() )
	{
	case Qt::Key_PageDown:
		mTimeLapse = false;
		break;
	case Qt::Key_PageUp:
		mTimeReverse = false;
		break;
	default:
		break;
	}
}


void World::updateSelf( const double & delta )
{
	mTimeOfDay += 0.002f * delta;

	if( mTimeLapse )
		mTimeOfDay += 0.2f * delta;
	if( mTimeReverse )
		mTimeOfDay -= 0.2f * delta;

	if( mTimeOfDay > 1.0f )
		mTimeOfDay -= 1.0f;

	mLevelTime += delta;
	if( mLevelTime >= mLevelDuration )
	{
		mLevelTime = 0.0f;
		mLevel++;
		player()->drawMessage(QString("Level %1").arg(level()));
		respawnEnemies();
		addRandomEnemy();
	}

	mSky->setTimeOfDay( mTimeOfDay );

	if( SplatterQuality::maximum() == SplatterQuality::MEDIUM )
		mSplatterSystem->setSplatBelow( true );
	else
		mSplatterSystem->setSplatBelow( false );
	mSplatterSystem->update( delta );
}


void World::updateSelfPost( const double & delta )
{

}


void World::drawSelf()
{
	int light = 0;
	QList< ALightSource * >::iterator i;
	for( i = mLightSources.begin(); i != mLightSources.end(); ++i )
	{
		(*i)->updateLightSource( GL_LIGHT0+light );
		light++;
	}

	glFog( GL_FOG_COLOR, mSky->baseColor() );
	glFog( GL_FOG_START, scene()->eye()->farPlane()*0.9f );
	glFog( GL_FOG_END, scene()->eye()->farPlane()*1.1f );
}


void World::drawSelfPost()
{
	mSplatterSystem->draw( modelViewMatrix() );
}


void World::SplatterInteractor::particleInteraction( const double & delta, ParticleSystem::Particle & particle )
{
	bool belowWater = false;
	bool belowGround = false;

	if( particle.position().y() - mWorld.landscape()->waterHeight() < -mWorld.splatterSystem()->particleSystem()->size()/2.0f )
		belowWater = true;
	if( mWorld.landscape()->terrain()->getHeightAboveGround( particle.position() ) < -mWorld.splatterSystem()->particleSystem()->size()/2.0f )
		belowGround = true;

	if( belowWater )
	{
		particle.rVelocity() -= (mWorld.splatterSystem()->particleSystem()->gravity()/1.1) * delta;
	}

	if( belowGround )
	{
		particle.setLife( 0.0f );
		if( !belowWater && SplatterQuality::maximum() == SplatterQuality::HIGH )
			mWorld.splatterSystem()->splat( particle.position(), mWorld.splatterSystem()->particleSystem()->size() * RandomNumber::minMax( 0.5f, 2.0f ) );
	}
}

void World::respawnEnemies()
{
	for( int i = 0; i < mEnemies.length(); ++i )
	{
		if( mEnemies.at(i).data()->state() == ACreature::DEAD )
		{
			mEnemies.at(i)->setState( ACreature::SPAWNING );
		}
	}
}

void World::addRandomEnemy()
{
	/*
	QSharedPointer<ACreature> newEnemy;
	switch( qrand()%2 )
	{
		case 0:
			newEnemy = QSharedPointer<ACreature>(new Splatterling( this, 0.25*(mLevel*0.25) ));
			break;
		case 1:
			//TODO: implement bug; uncomment next line; increase modulo in switch above
			newEnemy = QSharedPointer<ACreature>(new Splatterbug(this, 0.5f*(mLevel*5.0f)));
			break;
	}
	if( newEnemy.isNull() )
		return;
	mEnemies.append( newEnemy );
	add( newEnemy );
	*/
}
