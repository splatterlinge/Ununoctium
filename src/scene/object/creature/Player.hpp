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

#ifndef SCENE_OBJECT_PLAYER_INCLUDED
#define SCENE_OBJECT_PLAYER_INCLUDED


#include "ACreature.hpp"
#include "../Teapot.hpp"
#include "../Torch.hpp"
#include "../weapon/AWeapon.hpp"
#include <scene/AKeyListener.hpp>
#include <scene/AMouseListener.hpp>

#include <GLWidget.hpp>
#include <QTime>


class Player : public ACreature, public AKeyListener, public AMouseListener
{
public:
	Player( World * world );
	virtual ~Player();

	virtual void updateSelf( const double & delta );
	virtual void update2Self( const double & delta );
	virtual void drawSelf();
	virtual void draw2Self();

	virtual void keyPressEvent( QKeyEvent * event );
	virtual void keyReleaseEvent( QKeyEvent * event );

	virtual void mousePressEvent( QGraphicsSceneMouseEvent * event );
	virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event );
	virtual void mouseMoveEvent( MouseMoveEvent * event );
	virtual void mouseWheelEvent( QGraphicsSceneWheelEvent * event );

	virtual void receiveDamage( int damage, const QVector3D * position=NULL, const QVector3D * direction=NULL );
	virtual void receivePoints( int points );

	const int & armor() const { return mArmor; }
	const int & points() const { return mPoints; }
	const QString usageText() const { return mUsageText; }
	const float & killTime() const { return mKillTimer; }
	const float time() const { return mAliveTimer; }
	QSharedPointer<AWeapon> currentWeapon() { return mCurrentWeapon; }
	const QList< QSharedPointer<AWeapon> > & weapons() const { return mWeapons; }
	void giveWeapon( QSharedPointer<AWeapon> weapon );
	QSharedPointer<Torch> getTorch() { return mTorch; }

	void setArmor( const int & armor ) { mArmor = armor; }

	const QString textMessage() const { return mTextMessage; }
	const float textZoom() const { return mTextZoom; }
	const int textFade() const { return mTextFade; }
	const float textTime() const { return mTextTime; }

	void drawMessage( const QString message );

protected:

private:
	bool mTargetAvailable;
	QVector3D mTarget;
	QVector3D mTargetNormal;
	float mTargetDistance;

	QPointF mMouseDelta;
	bool mForwardPressed;
	bool mBackwardPressed;
	bool mLeftPressed;
	bool mRightPressed;
	bool mUpPressed;
	bool mDownPressed;
	bool mReloadPressed;
	bool mSpeedPressed;
	bool mUsePressed;
	bool mGodMode;
	bool mOnGround;
	bool mUnderWater;
	float mWaterImmersion;
	int mArmor;
	float mVelocityY;
	float mHeightAboveGround;
	float mAxisRotationX;
	float mAxisRotationY;
	float mSpeedCounter;
	float mSpeedCooldown;
	bool mDragTeapot;
	QVector3D mGroundNormal;
	QSharedPointer<Torch> mTorch;
	QList< QSharedPointer<AWeapon> > mWeapons;
	QSharedPointer<AWeapon> mCurrentWeapon;
	int mPoints;
	float mAliveTimer;
	float mKillTimer;
	float mTorchTimer;
	QString mUsageText;

	QString mTextMessage;
	float mTextZoom;
	int mTextFade;
	float mTextTime;

	void updateRotation( const double & delta );
	void updatePosition( const double & delta );
	void updateTarget( const double & delta );
	void updateTorch( const double & delta );

	void drawCrosshair();
};


#endif
