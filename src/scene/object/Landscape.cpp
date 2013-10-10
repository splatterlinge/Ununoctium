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

#include "Landscape.hpp"

#include "environment/Forest.hpp"
#include "environment/Grass.hpp"
#include "environment/PowerUp.hpp"
#include "environment/Flower.hpp"

#include <scene/Scene.hpp>
#include <scene/TextureRenderer.hpp>
#include <geometry/Terrain.hpp>

#include <resource/Material.hpp>
#include <resource/Shader.hpp>

#include <QString>
#include <QSettings>
#include <QGLShaderProgram>


int Landscape::Blob::sQuality = 0;


Landscape::Landscape( World * world, QString name ) :
	AWorldObject( world )
{
	mName = name;
	mDrawingReflection = false;
	mDrawingRefraction = false;

	QSettings s( "./data/landscape/"+name+"/landscape.ini", QSettings::IniFormat );

	s.beginGroup( "Terrain" );
		QString heightMapPath = s.value( "heightMapPath", "height.png" ).toString();
		mTerrainSize = QVector3D(
			s.value( "sizeX", 1000.0f ).toFloat(),
			s.value( "sizeY", 100.0f ).toFloat(),
			s.value( "sizeZ", 1000.0f ).toFloat()
		);
		mTerrainOffset = QVector3D(
			s.value( "offsetX", -500.0f ).toFloat(),
			s.value( "offsetY", -50.0f ).toFloat(),
			s.value( "offsetZ", -500.0f ).toFloat()
		);
		QString terrainMaterial = s.value( "material" ).toString();
		mTerrainMaterialScale = QVector2D(
			s.value( "materialScaleS", 1.0f ).toFloat(),
			s.value( "materialScaleT", 1.0f ).toFloat()
		);
		int smoothingPasses = s.value( "smoothingPasses", 1 ).toInt();
	s.endGroup();
	mTerrain = new Terrain( "./data/landscape/"+name+'/'+heightMapPath, mTerrainSize, mTerrainOffset, smoothingPasses );
	mTerrainFilter = new Filter( this, QSize( 3, 3 ) );
	mTerrainMaterial = new Material( scene()->glWidget(), terrainMaterial );

	s.beginGroup( "Water" );
		mWaterHeight = s.value( "height", 0.0f ).toFloat();
		mWaterClippingPlaneOffset = s.value( "clippingPlaneOffset", 0.01f ).toFloat();
	s.endGroup();
	mWaterShader = new Shader( scene()->glWidget(), "water" );
	QImage waterImage( "./data/landscape/"+name+'/'+"water.png" );
	if( waterImage.isNull() )
	{
		qFatal( "\"%s\" not found!", qPrintable("./data/landscape/"+name+'/'+"water.png") );
	}
	mWaterMap = scene()->glWidget()->bindTexture( waterImage );
	mReflectionRenderer = new TextureRenderer( scene()->glWidget(), QSize(512,512), true );
	mRefractionRenderer = new TextureRenderer( scene()->glWidget(), QSize(512,512), true );

	int blobNum = s.beginReadArray( "Blob" );
		for( int i=0; i<blobNum; i++ )
		{
			s.setArrayIndex( i );
			Blob * b = new Blob( this,
				s.value("rect").toRect(),
				s.value("material").toString(),
				QVector2D(
					s.value( "materialScaleS", 1.0f ).toFloat(),
					s.value( "materialScaleT", 1.0f ).toFloat()
				),
				s.value("priority").toInt(),
				"./data/landscape/"+name+'/'+s.value("maskPath").toString()
			);
			mBlobs.append( b );
		}
	s.endArray();

	int vegeNum = s.beginReadArray( "Vegetation" );
		for( int i=0; i<vegeNum; i++ )
		{
			s.setArrayIndex( i );
			QSharedPointer<AObject> f;
			QString type = s.value("type").toString();
			if( type == "forest" )
			{
				f = QSharedPointer<AObject>( new Forest( this,
					s.value("model").toString(), s.value("position").toPoint(),
					s.value("radius").toInt(), s.value("number").toInt(), s.value("priority").toInt() ) );
			}
			else if( type == "grass" )
			{
				f = QSharedPointer<AObject>( new Grass( this,
					s.value("model").toString(), s.value("position").toPoint(),
					s.value("radius").toInt(), s.value("number").toInt(), s.value("priority").toInt()) );
			}
			else if( type == "flower" )
			{
				f = QSharedPointer<AObject>( new Flower( this,
					s.value("model").toString(), s.value("position").toPoint(),
					s.value("radius").toInt(), s.value("number").toInt(), s.value("priority").toInt()) );
				mFlower = f;
			}

			mVegetation.append( f );
			add( f );
		}
	s.endArray();

	int powerNum = s.beginReadArray( "PowerUp" );
		for( int i=0; i<powerNum; i++ )
		{
			s.setArrayIndex( i );
			QSharedPointer<AObject> f;
			f = QSharedPointer<AObject>( new PowerUp( this,
				s.value("type").toString(),
				s.value("position").toPoint(),
				s.value("radius").toInt() ) );

			mPowerUps.append( f );
			add( f );
		}
	s.endArray();
}


Landscape::~Landscape()
{
	for( int i = 0; i < mBlobs.size(); ++i )
	{
		delete mBlobs[i];
	}
	delete mTerrain;
	delete mTerrainFilter;
	delete mTerrainMaterial;
	delete mReflectionRenderer;
	delete mRefractionRenderer;
	delete mWaterShader;
}


void Landscape::updateSelf( const double & delta )
{
	mTerrainFilter->update();
}


void Landscape::drawSelf()
{
}


void Landscape::drawSelfPost()
{
	mTerrainFilter->draw();

	mTerrainMaterial->bind();
	drawInfinitePlane( mTerrainOffset.y() );
	mTerrainMaterial->release();
}


void Landscape::drawInfinitePlane( const float & height )
{
	QVector2D groundPlaneFrom
	(
		scene()->eye()->position().x() - scene()->eye()->farPlane(),
		scene()->eye()->position().z() - scene()->eye()->farPlane()
	);
	QVector2D groundPlaneTo
	(
		scene()->eye()->position().x() + scene()->eye()->farPlane(),
		scene()->eye()->position().z() + scene()->eye()->farPlane()
	);
	glMatrixMode( GL_TEXTURE );	glActiveTexture( GL_TEXTURE0 );	glPushMatrix();
	glScalef( mTerrainMaterialScale.x()*mTerrain->toMapFactor().width(), -mTerrainMaterialScale.y()*mTerrain->toMapFactor().width(), 1.0f );
	glMatrixMode( GL_MODELVIEW );

	glNormal3f( 0, 1, 0 );
	glBegin( GL_QUADS );
		glTexCoord2f( groundPlaneFrom.x(),   groundPlaneTo.y() );	glVertex3f( groundPlaneFrom.x(), height,   groundPlaneTo.y() );
		glTexCoord2f(   groundPlaneTo.x(),   groundPlaneTo.y() );	glVertex3f(   groundPlaneTo.x(), height,   groundPlaneTo.y() );
		glTexCoord2f(   groundPlaneTo.x(), groundPlaneFrom.y() );	glVertex3f(   groundPlaneTo.x(), height, groundPlaneFrom.y() );
		glTexCoord2f( groundPlaneFrom.x(), groundPlaneFrom.y() );	glVertex3f( groundPlaneFrom.x(), height, groundPlaneFrom.y() );
	glEnd();

	glMatrixMode( GL_TEXTURE );	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}


void Landscape::renderReflection()
{
	mDrawingReflection = true;
	mReflectionRenderer->bind();
	glClear( GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_PROJECTION );	glPushMatrix();	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );	glPushMatrix();	glLoadIdentity();

	Eye * sceneEye = scene()->eye();

	Eye mirroredEye( *sceneEye );
	mirroredEye.detach();
	QQuaternion r = mirroredEye.rotation();
	r.setX( -r.x() );
	r.setZ( -r.z() );
	mirroredEye.setRotation( r );
	mirroredEye.setPositionY( -mirroredEye.position().y() + 2*mWaterHeight);
	mirroredEye.setScale( QVector3D( 1,-1, 1 ) );
	if( scene()->eye()->position().y()>mWaterHeight )
		mirroredEye.setClippingPlane( 0, QVector4D(0,1,0, -mWaterHeight+mWaterClippingPlaneOffset) );
	else
		mirroredEye.setClippingPlane( 0, QVector4D(0,-1,0, mWaterHeight+mWaterClippingPlaneOffset) );
	scene()->setEye( &mirroredEye );

	glFrontFace( GL_CW );
	scene()->eye()->applyGL();
	scene()->eye()->enableClippingPlanes();
	scene()->root()->draw();
	scene()->root()->draw2();
	glFrontFace( GL_CCW );
	scene()->eye()->setClippingPlane( 0 );

	scene()->setEye( sceneEye );

	glMatrixMode( GL_PROJECTION ); glPopMatrix();
	glMatrixMode( GL_MODELVIEW ); glPopMatrix();
	mReflectionRenderer->release();
	mDrawingReflection = false;
}


void Landscape::renderRefraction()
{
	mDrawingRefraction = true;
	mRefractionRenderer->bind();
	glClear( GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_PROJECTION );	glPushMatrix();	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );	glPushMatrix();	glLoadIdentity();
	if( scene()->eye()->position().y()>mWaterHeight )
		scene()->eye()->setClippingPlane( 0, QVector4D(0,-1,0, mWaterHeight+mWaterClippingPlaneOffset) );
	else
		scene()->eye()->setClippingPlane( 0, QVector4D(0,1,0, -mWaterHeight+mWaterClippingPlaneOffset) );

	scene()->eye()->applyGL();
	scene()->eye()->enableClippingPlanes();
	scene()->root()->draw();
	scene()->root()->draw2();
	scene()->eye()->setClippingPlane( 0 );

	glMatrixMode( GL_PROJECTION );	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );	glPopMatrix();
	mRefractionRenderer->release();
	mDrawingRefraction = false;
}


void Landscape::draw2SelfPost()
{
	if( mDrawingReflection || mDrawingRefraction )
		return;

	MaterialQuality::Type defaultQuality = MaterialQuality::maximum();
	MaterialQuality::setMaximum( MaterialQuality::LOW );
	renderReflection();
	renderRefraction();
	MaterialQuality::setMaximum( defaultQuality );

	glDisable( GL_CULL_FACE );
	mWaterShader->bind();

	mWaterShader->program()->setUniformValue( "eyeVector", world()->player()->eyeDirection() );
	mWaterShader->program()->setUniformValue( "reflectionMap", 0 );
	mWaterShader->program()->setUniformValue( "refractionMap", 1 );
	mWaterShader->program()->setUniformValue( "waterMap", 2 );
	mWaterShader->program()->setUniformValue( "time", world()->sky()->timeOfDay() );
	glActiveTexture( GL_TEXTURE2 );	glBindTexture( GL_TEXTURE_2D, mWaterMap );
	glActiveTexture( GL_TEXTURE1 );	glBindTexture( GL_TEXTURE_2D, mRefractionRenderer->texID() );
	glActiveTexture( GL_TEXTURE0 );	glBindTexture( GL_TEXTURE_2D, mReflectionRenderer->texID() );
	drawInfinitePlane( mWaterHeight );
	mWaterShader->release();
	glActiveTexture( GL_TEXTURE2 );	glBindTexture( GL_TEXTURE_2D, 0 );
	glActiveTexture( GL_TEXTURE1 );	glBindTexture( GL_TEXTURE_2D, 0 );
	glActiveTexture( GL_TEXTURE0 );	glBindTexture( GL_TEXTURE_2D, 0 );
	glEnable( GL_CULL_FACE );
}


void Landscape::drawPatch( const QRectF & rect )
{
	mTerrainMaterial->bind();
	glMatrixMode( GL_TEXTURE );	glActiveTexture( GL_TEXTURE0 );	glPushMatrix();
		glScalef( mTerrainMaterialScale.x(), -mTerrainMaterialScale.y(), 1.0f );
		glMatrixMode( GL_MODELVIEW );
		mTerrain->drawPatch( rect );
	glMatrixMode( GL_TEXTURE );	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	mTerrainMaterial->release();

	glDepthMask( GL_FALSE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	for( int i = 0; i < mBlobs.size(); ++i )
	{
		mBlobs[i]->drawPatch( rect );
	}
	glDisable( GL_BLEND );
	glDepthMask( GL_TRUE );
}


const AObject * Landscape::intersectLine( const AObject * exclude, const QVector3D & origin, const QVector3D & direction, float & length, QVector3D * normal ) const
{
	const AObject * nearestTarget = AObject::intersectLine( exclude, origin, direction, length, normal );
	if( mTerrain->intersectLine( origin, direction, length, normal ) )
		nearestTarget = this;
	return nearestTarget;
}


QVector<const AObject*> Landscape::collideSphere( const AObject * exclude, const float & radius, QVector3D & center, QVector3D * normal ) const
{
	QVector<const AObject*> collides = AObject::collideSphere( exclude, radius, center, normal );

	float landscapeHeight;
	if( mTerrain->getHeight( center, landscapeHeight ) )
	{
		float depth = landscapeHeight + radius - center.y();
		if( depth > 0.0f )
		{
			collides.append( this );
			center += QVector3D( 0, depth, 0 );
			if( normal )
				*normal += mTerrain->getNormal( center );
		}
	}
	else
	{
		float depth = mTerrainOffset.y() + radius - center.y();
		if( depth > 0.0f )
		{
			collides.append( this );
			center += QVector3D( 0, depth, 0 );
			if( normal )
				*normal += QVector3D( 0, 1, 0 );
		}
	}
	return collides;
}




////////////////////////////////////////////////////////////////
// Blob


Landscape::Blob::Blob( Landscape * landscape, QRect rect, QString materialName, QVector2D materialScale, int priority, QString blobMapPath )
{
	mGLWidget = landscape->scene()->glWidget();
	mLandscape = landscape;
	mRect = rect;
	mMaterialScale = materialScale;
	mMaterial = new Material( mGLWidget, materialName, MaterialShaderVariant::BLOBBING );
	QImage blobMap = QImage( blobMapPath );
	if( blobMap.isNull() )
	{
		qFatal( "BlobMap from file \"%s\" could not be loaded!", blobMapPath.toLocal8Bit().constData() );
	}
	mBlobMap =  mGLWidget->bindTexture( blobMap );
	mMaterial->setBlobMap( mBlobMap );
	mPriority = priority;
}


Landscape::Blob::~Blob()
{
	mGLWidget->deleteTexture( mBlobMap );
	delete mMaterial;
}


void Landscape::Blob::drawPatchMap( const QRect & visible )
{
	if( mPriority >= 99-sQuality)
	{
		QRect rectToDraw = mRect.intersected( visible );
		if( rectToDraw.width() <= 1 || rectToDraw.height() <= 1 )
			return;	// nothing to draw

		mMaterial->bind();
		glMatrixMode( GL_TEXTURE );

		glActiveTexture( GL_TEXTURE0 );	glPushMatrix();
		glScaled( mMaterialScale.x(), -mMaterialScale.y(), 1.0 );

		glActiveTexture( GL_TEXTURE1 );	glPushMatrix();
		glScaled( 1.0/((double)mRect.width()), -1.0/((double)mRect.height()), 1.0 );
		glTranslated( -mRect.x(), -mRect.y(), 0.0 );

		mLandscape->terrain()->drawPatchMap( rectToDraw );

		glMatrixMode( GL_TEXTURE );
		glActiveTexture( GL_TEXTURE1 );	glPopMatrix();
		glActiveTexture( GL_TEXTURE0 );	glPopMatrix();

		glMatrixMode( GL_MODELVIEW );
		mMaterial->release();
	}
}




////////////////////////////////////////////////////////////////
// Filter


Landscape::Filter::Patch::Patch( QVector3D position, QVector3D size ) :
	mPosition(position),
	mSize(size)
{
	mCenter = mPosition + mSize/2.0;
	mBoundingSphereRadius = size.length()/2.0f;
}


Landscape::Filter::Filter( Landscape * landscape, QSize filterSize ) :
	mLandscape( landscape ),
	mFilterSize( filterSize )
{
	mPatches.resize( filterSize.width() * filterSize.height() );
}


Landscape::Filter::~Filter()
{
}


void Landscape::Filter::update()
{
	/* example for filterSize 3x3
	 *
	 * .---------size.x()---------.
	 *                   .-stepX--.
	 * |--------|--------|--------|-    -
	 * |        |        |        | |    |
	 * |        |        |        |stepZ |
	 * |        |        |        | |    |
	 * |--------|--------|--------|-     |
	 * |        |        |        |      |
	 * |        |  Eye   |        |     size.z()
	 * |        |        |        |      |
	 * |--------|--------|--------|      |
	 * |        |        |        |      |
	 * |        |        |        |      |
	 * |        |        |        |      |
	 * |--------|--------|--------|     -
	 * ^             '--farPlane--'
	 * from
	 */
	//TODO: maybe do these calculations in map coordinates ...
	float farPlane = mLandscape->scene()->eye()->farPlane();
	QVector3D from = QVector3D( -farPlane, mLandscape->terrain()->offset().y(), -farPlane );
	QVector3D size = QVector3D( 2.0f*farPlane, mLandscape->terrain()->size().y(), 2.0f*farPlane );
	float stepX =  size.x()/(float)mFilterSize.width();
	float stepZ =  size.z()/(float)mFilterSize.height();
	QVector3D patchSize = QVector3D( stepX, mLandscape->terrain()->size().y(), stepZ );
	for( int z=0; z<mFilterSize.height(); ++z )
	{
		for( int x=0; x<mFilterSize.width(); ++x )
		{
			QVector3D pos = from + QVector3D( (float)x*stepX, 0.0f, (float)z*stepZ );
			mPatches[z*mFilterSize.width()+x] = Patch( pos, patchSize );
		}
	}
}


void Landscape::Filter::draw()
{
	FrustumTest frustumTest;
	frustumTest.sync();
	for( int z=0; z<mFilterSize.height(); ++z )
	{
		QRectF mergedRect;	// we will merge patches along the x axis as this is optimal for the terrain mesh VBO
		for( int x=0; x<mFilterSize.width(); ++x )
		{
			Patch & patch = mPatches[z*mFilterSize.width()+x];
			QVector3D eyePosition = QVector3D( mLandscape->scene()->eye()->position().x(), 0.0f, mLandscape->scene()->eye()->position().z() );
			QVector3D spherePosition = eyePosition + patch.center();
			/*
			GLUquadric * q = gluNewQuadric();
			glPushAttrib( GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT );
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			glDisable( GL_LIGHTING );
			glColor3f( 1, 1, 1 );
			glPushMatrix();
			glTranslate( spherePosition );
			glDisable( GL_CULL_FACE );
			gluSphere( q, patch.boundingSphereRadius(), 32, 32 );
			glEnable( GL_CULL_FACE );
			glPopMatrix();
			glPopAttrib();
			gluDeleteQuadric( q );
			*/
			if( frustumTest.isSphereInFrustum( spherePosition, patch.boundingSphereRadius() ) )
			{
				mergedRect = mergedRect.united(
					QRectF(
						eyePosition.x() + patch.position().x(),
						eyePosition.z() + patch.position().z(),
						patch.size().x(),
						patch.size().z()
					)
				);
			}
		}
		if( !mergedRect.isNull() )
		{
			mLandscape->drawPatch( mergedRect );
		}
	}
}
