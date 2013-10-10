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

#include "Torch.hpp"
#include "World.hpp"

#include <scene/TextureRenderer.hpp>
#include <scene/Scene.hpp>
#include <resource/Material.hpp>
#include <resource/StaticModel.hpp>


const GLfloat Torch::sQuadVertices[] =
{	// positions		texcoords
	-1.0,  1.0,  0.0,	0.0, 1.0,
	-1.0, -1.0,  0.0,	0.0, 0.0,
	 1.0, -1.0,  0.0,	1.0, 0.0,
	 1.0,  1.0,  0.0,	1.0, 1.0
};

QGLBuffer Torch::sQuadVertexBuffer;


Torch::Torch( World * world ) :
	AWorldObject( world )
{
	mFlareSize = 5.0f;
	mColorCycle = 0.0f;
	mFlareRotation = 0.0f;
	mFlarePosition = QVector3D(0,1.6,0);

	setBoundingSphere( mFlareSize + mFlarePosition.length() );

	if( !sQuadVertexBuffer.isCreated() )
	{
		sQuadVertexBuffer = QGLBuffer( QGLBuffer::VertexBuffer );
		sQuadVertexBuffer.create();
		sQuadVertexBuffer.bind();
		sQuadVertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
		sQuadVertexBuffer.allocate( sQuadVertices, sizeof(sQuadVertices) );
		sQuadVertexBuffer.release();
	}

	mMaterial = new Material( scene()->glWidget(), "Flare" );
	world->addLightSource( this );

	mModel = new StaticModel( scene()->glWidget(), "SpearTorch" );
}


Torch::~Torch()
{
	delete mMaterial;
	world()->removeLightSource( this );
}


void Torch::updateLightSource( GLenum light )
{
	glLight( light, GL_POSITION, QVector4D(	pointToWorld(mFlarePosition), 1	) );
	glLight( light, GL_CONSTANT_ATTENUATION, 0.0f );
	glLight( light, GL_LINEAR_ATTENUATION, 0.05f );
	glLight( light, GL_QUADRATIC_ATTENUATION, 0.0f );
	if( parent() )
	{
		glLight( light, GL_AMBIENT, QVector4D(	0, 0, 0, 1	) );
		glLight( light, GL_DIFFUSE, QVector4D(	color()	) );
		glLight( light, GL_SPECULAR, QVector4D(	color()	) );
		glEnable( light );
	}
	else
	{
		glLight( light, GL_AMBIENT, QVector4D(	0, 0, 0, 1	) );
		glLight( light, GL_DIFFUSE, QVector4D(	0, 0, 0, 1	) );
		glLight( light, GL_SPECULAR, QVector4D(	0, 0, 0, 1	) );
		glDisable( light );
	}
}


void Torch::updateSelf( const double & delta )
{
	QColor color = QColor::fromHsvF( mColorCycle, 0.5f, 1.0f, 1.0f );
	mColorCycle += 0.0005f;
	if( mColorCycle >= 1.0f )
		mColorCycle -= 1.0f;
	mColor = QVector4D( color.redF(), color.greenF(), color.blueF(), 1.0f );

	mFlareRotation += 20.0f * delta;
}


void Torch::drawSelf()
{
	mModel->draw();
}


void Torch::draw2Self()
{
	if( world()->landscape()->drawingReflection() || world()->landscape()->drawingRefraction() )
		return;

	const unsigned char samplingPoints = 16;
	unsigned char visiblePoints;

	glPushMatrix();
	glTranslate( mFlarePosition );
	glScale( 0.3f );
	visiblePoints = mOcclusionTest.randomPointsOnUnitSphereVisible( samplingPoints );
	glPopMatrix();
	if( !visiblePoints )
		return;

	glPushAttrib( GL_VIEWPORT_BIT | GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT );
	glDepthMask( GL_FALSE );
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );

	glDisable( GL_LIGHTING );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	mMaterial->bind();
	glColor( ((float)visiblePoints/(float)samplingPoints)*mColor );

	sQuadVertexBuffer.bind();
	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 3, GL_FLOAT, 5*sizeof(GLfloat), (void*)0 );
	glTexCoordPointer( 2, GL_FLOAT, 5*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)) );

	QMatrix4x4 flareCenter = modelViewMatrix();
	flareCenter.translate( mFlarePosition );
	Bilboard::begin( flareCenter );
	glScale( mFlareSize );
	glRotate( mFlareRotation, QVector3D(0,0,1) );
	glDrawArrays( GL_QUADS, 0, 4 );
	glRotate( -mFlareRotation*2.7f, QVector3D(0,0,1) );
	glDrawArrays( GL_QUADS, 0, 4 );
	Bilboard::end();

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
	sQuadVertexBuffer.release();

	mMaterial->release();
	glDisable( GL_BLEND );

	glPopAttrib();
}
