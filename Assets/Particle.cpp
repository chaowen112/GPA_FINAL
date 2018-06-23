/*
 *  Particle.cpp
 *  Graphics Assignment Four
 *
 *  Created by Karl Grogan on 26/04/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Particle.h"
#include <stdlib.h>
#include <GLUT/glut.h>

// Constructor for a particle.
void Particle::CreateParticle()
{
	lifetime= (float)rand();
	decay=1;
	r = 0.7;
	g = 0.7;
	b = 1.0;
	xpos= 256.0;
	ypos= 128.0;
	zpos= 5.0;
	xspeed = 2-(int)rand() % 5 ;
	yspeed = 2-(int)rand() % 5  ;
	zspeed = 2- (int)rand() % 5  ;
	active = true;
}
	
/* 
 Evolves the particle parameters over time.
 This method changes the vertical and horizontal poition of the particle, its, speed and decay time.
*/
void Particle::EvolveParticle()
{
	lifetime-=decay;
	xpos+= 2- (int)rand()  % 5 ;
	ypos-= 2-(int)rand()  % 5 ;
	zpos= 0.1;
	yspeed+= 2- (int)rand()  % 5 ;
}

/*
 This method draws the particle on screen.
 Sets the clorour mode.
 Sets the drawing primitive.
 Sets the coordinates of each point on the triangle strip
*/
void Particle::DrawObjects()
{
	
	
	if((active==true) && (lifetime>0.0))
	{
		glColor3f(r,g,b);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0,1.0); glVertex3f(xpos+3, ypos+3,zpos);     // top    right
		glTexCoord2f(0.0,1.0); glVertex3f(xpos-3, ypos+3,zpos);     // top    left
		glTexCoord2f(0.0,1.0); glVertex3f(xpos+3, ypos-3,zpos);     // bottom right
		glTexCoord2f(0.0,1.0); glVertex3f(xpos-3, ypos-3,zpos);     // bottom left

		glEnd();
	} 
}

// Getter and setter methods


float Particle::GetXPos()
{
	return xpos;
}

void Particle::SetXPos(float xPos)
{
	xpos = xPos;
}


void Particle::SetYPos(float yPos)
{
	xpos = yPos;
}

float Particle::GetYPos()
{
	return ypos;
}

void Particle::SetG(float myG)
{
	g = myG;
}

float Particle::GetG()
{
	return g;
}




