#ifndef PLAYBOX_ARBITER_H
#define PLAYBOX_ARBITER_H

#include "playbox2d/maths.h"
#include "playbox2d/body.h"
#include <stdint.h>

typedef union {
  struct PBEdges {
    char inEdge1;
    char outEdge1;
    char inEdge2;
    char outEdge2;
  } e;
  uint32_t value;
} PBFeaturePair;

typedef struct {
  PBVec2 position;
  PBVec2 normal;
  PBVec2 r1, r2;
  float separation;
  float Pn;  // accumulated normal impulse
  float Pt;  // accumulated tangent impulse
  float Pnb;  // accumulated normal impulse for position bias
  float massNormal, massTangent;
  float bias;
  PBFeaturePair feature;
} PBContact;

#ifndef MAX_ARBITER_POINTS
#define MAX_ARBITER_POINTS 2
#endif

typedef struct {
  // Connectivity
  PBBody* body1;
  PBBody* body2;

  // Combined friction
  float friction;
  
  // Run-time data
  int numContacts;
  PBContact contacts[MAX_ARBITER_POINTS];
} PBArbiter;

extern PBArbiter* PBArbiterCreate(PBBody* body1, PBBody* body2);
extern void PBArbiterFree(PBArbiter* arbiter);
extern void PBArbiterUpdate(PBArbiter* arbiter, PBContact* newContacts, int numNewContacts);
extern void PBArbiterPreStep(PBArbiter* arbiter, float inv_dt);
extern void PBArbiterApplyImpulse(PBArbiter* arbiter);

extern int PBCollide(PBContact* contacts, PBBody* body1, PBBody* body2);

#endif