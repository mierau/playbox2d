#include "playbox2d/playbox2d.h"
#include "playbox2d/joint.h"
#include "playbox2d/body.h"
#include "playbox2d/maths.h"

PBJoint* PBJointCreate(PBBody* b1, PBBody* b2, const PBVec2 anchor) {
  PBJoint* joint = pb_alloc(sizeof(PBJoint));
  memset(joint, 0, sizeof(PBJoint));
  
  joint->body1 = b1;
  joint->body2 = b2;
  
  PBMat22 Rot1T = PBMat22Transpose(PBMat22MakeWithAngle(joint->body1->rotation));
  PBMat22 Rot2T = PBMat22Transpose(PBMat22MakeWithAngle(joint->body2->rotation));
  
  joint->localAnchor1 = PBMat22MultVec(Rot1T, PBVec2Sub(anchor, joint->body1->position));
  joint->localAnchor2 = PBMat22MultVec(Rot2T, PBVec2Sub(anchor, joint->body2->position));
  
  joint->P.x = 0.0f;
  joint->P.y = 0.0f;
  
  joint->softness = 0.0f;
  joint->biasFactor = 0.2f;
  
  return joint;
}

PBJoint* PBJointCreateEmpty(void) {
  PBJoint* joint = pb_alloc(sizeof(PBJoint));
  memset(joint, 0, sizeof(PBJoint));
  joint->biasFactor = 0.2f;
  return joint;
}

void PBJointFree(PBJoint* joint) {
  pb_free(joint);
}

void PBJointPreStep(PBJoint* joint, float inv_dt) {
  PBBody* body1 = joint->body1;
  PBBody* body2 = joint->body2;

  PBMat22 Rot1 = PBMat22MakeWithAngle(body1->rotation);
  PBMat22 Rot2 = PBMat22MakeWithAngle(body2->rotation);
  
  joint->r1 = PBMat22MultVec(Rot1, joint->localAnchor1);
  joint->r2 = PBMat22MultVec(Rot2, joint->localAnchor2);
  
  PBMat22 K1;
  K1.col1.x = body1->invMass + body2->invMass; K1.col2.x = 0.0f;
  K1.col1.y = 0.0f; K1.col2.y = body1->invMass + body2->invMass;
  
  PBMat22 K2;
  K2.col1.x =  body1->invI * joint->r1.y * joint->r1.y; K2.col2.x = -body1->invI * joint->r1.x * joint->r1.y;
  K2.col1.y = -body1->invI * joint->r1.x * joint->r1.y; K2.col2.y =  body1->invI * joint->r1.x * joint->r1.x;

  PBMat22 K3;
  K3.col1.x =  body2->invI * joint->r2.y * joint->r2.y; K3.col2.x = -body2->invI * joint->r2.x * joint->r2.y;
  K3.col1.y = -body2->invI * joint->r2.x * joint->r2.y; K3.col2.y =  body2->invI * joint->r2.x * joint->r2.x;

  PBMat22 K = PBMat22Add(PBMat22Add(K1, K2), K3);
  K.col1.x += joint->softness;
  K.col2.y += joint->softness;
  
  joint->M = PBMat22Invert(K);
  
  PBVec2 p1 = PBVec2Add(body1->position, joint->r1);
  PBVec2 p2 = PBVec2Add(body2->position, joint->r2);
  PBVec2 dp = PBVec2Sub(p2, p1);

  if(PBPositionCorrection) {
    joint->bias = PBVec2MultF(dp, -joint->biasFactor * inv_dt);
  }
  else {
    joint->bias.x = 0.0f;
    joint->bias.y = 0.0f;
  }
  
  if(PBWarmStarting) {
    // Apply accumulated impulse.
    body1->velocity = PBVec2Sub(body1->velocity, PBVec2MultF(joint->P, body1->invMass));
    body1->angularVelocity -= body1->invI * PBVec2Cross(joint->r1, joint->P);
    
    body2->velocity = PBVec2Add(body2->velocity, PBVec2MultF(joint->P, body2->invMass));
    body2->angularVelocity += body2->invI * PBVec2Cross(joint->r2, joint->P);
  }
  else {
    joint->P.x = 0.0f;
    joint->P.y = 0.0f;
  }
}

void PBJointApplyImpulse(PBJoint* joint) {
  PBBody* body1 = joint->body1;
  PBBody* body2 = joint->body2;
  
  PBVec2 dv = PBVec2Sub(PBVec2Sub(PBVec2Add(body2->velocity, PBVec2FCross(body2->angularVelocity, joint->r2)), body1->velocity), PBVec2FCross(body1->angularVelocity, joint->r1));

  PBVec2 impulse = PBMat22MultVec(joint->M, PBVec2Sub(PBVec2Sub(joint->bias, dv), PBVec2MultF(joint->P, joint->softness)));

  body1->velocity = PBVec2Sub(body1->velocity, PBVec2MultF(impulse, body1->invMass));
  body1->angularVelocity -= body1->invI * PBVec2Cross(joint->r1, impulse);

  body2->velocity = PBVec2Add(body2->velocity, PBVec2MultF(impulse, body2->invMass));
  body2->angularVelocity += body2->invI * PBVec2Cross(joint->r2, impulse);

  joint->P = PBVec2Add(joint->P, impulse);
}