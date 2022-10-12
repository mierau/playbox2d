#ifndef PLAYBOX2D_BODY_H
#define PLAYBOX2D_BODY_H

#include "playbox2d/maths.h"

typedef struct {
  // State
  PBVec2 position;
  float rotation;
  PBVec2 velocity;
  float angularVelocity;
  
  // Properties
  PBVec2 width;
  float AABBHalfSize;
  float friction;
  float mass, invMass;
  float I, invI;
  
  // Applied forces
  PBVec2 force;
  float torque;

  // Reference to world
  void* world;
} PBBody;

extern PBBody* PBBodyCreate(void);
extern void PBBodyFree(PBBody* body);
extern void PBBodySet(PBBody* body, const PBVec2 w, float m);
extern void PBBodyAddForce(PBBody* body, const PBVec2 f);

#endif