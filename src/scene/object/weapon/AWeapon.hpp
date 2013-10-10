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

#ifndef SCENE_OBJECT_WEAPON_AWEAPON_INCLUDED
#define SCENE_OBJECT_WEAPON_AWEAPON_INCLUDED


#include "../AWorldObject.hpp"


/// Abstract baseclass for a weapon
/**
 *
 */
class AWeapon : public AWorldObject
{
public:
	/// Creates a weapon for the given world
	AWeapon( World * world );
	AWeapon( World * world, int ammo, int clipammo, int clipsize );
	/// Frees this object
	virtual ~AWeapon();

	virtual void triggerPressed() = 0;
	virtual void triggerReleased() = 0;
	virtual void holster() = 0;
	virtual void pull() = 0;
	virtual bool isPulled() = 0;
	virtual void reload() {}
	virtual bool isReloading() { return false; }
	virtual void setTarget( const QVector3D * target ) {}

	const QString & name() const { return mName; }
	const int & ammo() const { return mAmmo; }
	const int & clipammo() const { return mClipAmmo; }
	const int & clipsize() const { return mClipSize; }

	void setAmmo( const int & ammo ) { mAmmo = ammo; }

protected:
	QQuaternion getRotationToTarget( const QVector3D * target, const float & maxRotationDP = 0.0f );

	QString mName;
	int mAmmo;
	int mClipAmmo;
	int mClipSize;
};


#endif
