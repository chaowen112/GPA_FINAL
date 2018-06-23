/*
 *  Particle.h
 *  Graphics Assignment Four
 *
 *  Created by Karl Grogan on 26/04/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

class Particle
{
private:
	float lifetime;                       // Lifetime of the particle
	float decay;                          // decay speed of the particle
	float r,g,b;                          // color values of the particle
	float xpos,ypos,zpos;                 // position of the particle
	float xspeed,yspeed,zspeed;           // speed of the particle
	bool active;						  // is particle active or not?
	
public:
    //Particle();
	void CreateParticle();
	void EvolveParticle();
	void DrawObjects();
	float GetXPos();
	void SetXPos(float xPos);
	void SetYPos(float yPos);
	float GetYPos();
	float GetG();
	void SetG(float myG);

};