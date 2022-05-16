#ifndef PLAYBOX_JOINT_H
#define PLAYBOX_JOINT_H

#include "playbox2d/maths.h"
#include "playbox2d/body.h"

typedef struct {
  PBMat22 M;
  PBVec2 localAnchor1, localAnchor2;
  PBVec2 r1, r2;
  PBVec2 bias;
  PBVec2 P;    // accumulated impulse
  PBBody* body1;
  PBBody* body2;
  float biasFactor;
  float softness;
  
  // Reference to world
  void* world;
} PBJoint;

extern PBJoint* PBJointCreate(PBBody* b1, PBBody* b2, const PBVec2 anchor);
extern PBJoint* PBJointCreateEmpty(void);
extern void PBJointFree(PBJoint* body);
extern void PBJointPreStep(PBJoint* joint, float inv_dt);
extern void PBJointApplyImpulse(PBJoint* joint);

#endif