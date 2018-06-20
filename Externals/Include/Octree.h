//
//  Octree.h
//  OpenGL 4
//
//  Created by Michael on 2/5/14.
//  Copyright (c) 2014 michael. All rights reserved.
//

#ifndef __OpenGL_4__Octree__
#define __OpenGL_4__Octree__

#include <iostream>
#include "GLM/glm/glm.hpp"

const int MAX_OCTREE_DEPTH = 6;
const int MAX_BALLS_PER_NODE = 6;
const int MIN_BALLS_PER_NODE = 3;

class Octree {
private:
    unsigned int size; //Size of current
    
    Octree *children[2][2][2]; //Splits itself into 8 separate trees, hence octree
    
    bool hasChildren;
    
    int *balls; //Ball number
    
    int depth; //How deep the octree is from the root
    int numBalls; //Number of balls in current tree
    
public:
    
    void fileBall(int ball, glm::vec3 pos, bool addBall);
    
    void haveChildren();
    
};



#endif /* defined(__OpenGL_4__Octree__) */
