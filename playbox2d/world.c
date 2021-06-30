#include "world.h"
#include "platform.h"
#include "arbiter.h"

int PBWorldFindArbiter(PBWorld* world, PBBody* body1, PBBody* body2);
int PBWorldFindFirstArbiterForBody(PBWorld* world, PBBody* body);
int PBWorldFindFirstJointForBody(PBWorld* world, PBBody* body);

PBBody* PBWorldGetBody(PBWorld* world, int i);
PBJoint* PBWorldGetJoint(PBWorld* world, int i);

PBWorld* PBWorldCreate(PBVec2 gravity, int iterations) {
  PBWorld* world = pb_alloc(sizeof(PBWorld));
  memset(world, 0, sizeof(PBWorld));
  
  world->gravity = gravity;
  world->iterations = iterations;
  
  world->bodies = PBArrayCreate(sizeof(size_t));
  world->joints = PBArrayCreate(sizeof(size_t));
  
  world->arbiters = PBArrayCreate(sizeof(PBArbiter));
  
  return world;
}

void PBWorldFree(PBWorld* world) {
  PBArrayFree(world->bodies);
  PBArrayFree(world->joints);
  PBArrayFree(world->arbiters);
  pb_free(world);
}

void PBWorldAddBody(PBWorld* world, PBBody* body) {
  size_t addr = (size_t)body;
  body->world = world;
  PBArrayAppendItem(world->bodies, &addr);
}

void PBWorldRemoveBody(PBWorld* world, PBBody* body) {
  size_t addr = (size_t)body;
  body->world = NULL;
  
  // Remove body
  PBArrayRemoveItem(world->bodies, &addr);
  
  // Remove all related arbiters
  int i = PBWorldFindFirstArbiterForBody(world, body);
  while(i != -1) {
    PBArrayRemoveItemAt(world->arbiters, i);
    i = PBWorldFindFirstArbiterForBody(world, body);
  }
  
  // Remove all related joints
  i = PBWorldFindFirstJointForBody(world, body);
  while(i != -1) {
    PBJoint* joint = PBWorldGetJoint(world, i);
    joint->world = NULL;
    PBArrayRemoveItemAt(world->joints, i);
    i = PBWorldFindFirstJointForBody(world, body);
  }
}

void PBWorldAddJoint(PBWorld* world, PBJoint* joint) {
  size_t addr = (size_t)joint;
  joint->world = world;
  PBArrayAppendItem(world->joints, &addr);
}

void PBWorldRemoveJoint(PBWorld* world, PBJoint* joint) {
  size_t addr = (size_t)joint;
  joint->world = NULL;
  PBArrayRemoveItem(world->joints, &addr);
}

void PBWorldClear(PBWorld* world) {
  PBArrayRemoveAllItems(world->bodies);
  PBArrayRemoveAllItems(world->joints);
}

inline PBBody* PBWorldGetBody(PBWorld* world, int i) {
  return (PBBody*)(*((size_t*)PBArrayGetItem(world->bodies, i)));
}

inline PBJoint* PBWorldGetJoint(PBWorld* world, int i) {
  return (PBJoint*)(*((size_t*)PBArrayGetItem(world->joints, i)));
}

int PBWorldFindFirstArbiterForBody(PBWorld* world, PBBody* body) {
  if(world->arbiters->count == 0) {
    return -1;
  }
    
  for(int i = 0; i < world->arbiters->count; i++) {
    PBArbiter* arbiter = (PBArbiter*)PBArrayGetItem(world->arbiters, i);
    if(arbiter->body1 == body || arbiter->body2 == body) {
      return i;
    }
  }
  
  return -1;
}

int PBWorldFindFirstJointForBody(PBWorld* world, PBBody* body) {
  if(world->joints->count == 0) {
    return -1;
  }
    
  for(int i = 0; i < world->joints->count; i++) {
    PBJoint* joint = PBWorldGetJoint(world, i);
    if(joint->body1 == body || joint->body2 == body) {
      return i;
    }
  }
  
  return -1;
}

int PBWorldFindArbiter(PBWorld* world, PBBody* body1, PBBody* body2) {
  if(world->arbiters->count == 0) {
    return -1;
  }
  
  PBBody* b1;
  PBBody* b2;
  if(body1 < body2) {
    b1 = body1;
    b2 = body2;
  }
  else {
    b1 = body2;
    b2 = body1;
  }
  
  for(int i = 0; i < world->arbiters->count; i++) {
    PBArbiter* arbiter = (PBArbiter*)PBArrayGetItem(world->arbiters, i);
    if(arbiter->body1 == b1 && arbiter->body2 == b2) {
      return i;
    }
  }
  
  return -1;
}

int PBWorldNumberOfContactsBetweenBodies(PBWorld* world, PBBody* body1, PBBody* body2) {
  if(world->arbiters->count == 0) {
    return 0;
  }
  
  PBBody* b1;
  PBBody* b2;
  if(body1 < body2) {
    b1 = body1;
    b2 = body2;
  }
  else {
    b1 = body2;
    b2 = body1;
  }
  
  for(int i = 0; i < world->arbiters->count; i++) {
    PBArbiter* arbiter = (PBArbiter*)PBArrayGetItem(world->arbiters, i);
    if(arbiter->body1 == b1 && arbiter->body2 == b2) {
      return arbiter->numContacts;
    }
  }
  
  return 0;
}

void PBWorldStep(PBWorld* world, float dt) {
  float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;

  // Determine overlapping bodies and update contact points.
  PBWorldBroadphase(world);

  // Integrate forces.
  for(int i = 0; i < world->bodies->count; ++i) {
    PBBody* b = PBWorldGetBody(world, i);

    if(b->invMass == 0.0f) {
      continue;
    }

    b->velocity = PBVec2Add(b->velocity, PBVec2MultF(PBVec2Add(world->gravity, PBVec2MultF(b->force, b->invMass)), dt));
    b->angularVelocity += dt * b->invI * b->torque;
  }

  // Perform pre-steps.
  for(int i = 0; i < world->arbiters->count; i++) {
    PBArbiter* arbiter = (PBArbiter*)PBArrayGetItem(world->arbiters, i);
    PBArbiterPreStep(arbiter, inv_dt);
  }

  for(int i = 0; i < world->joints->count; i++) {
    PBJoint* joint = PBWorldGetJoint(world, i);
    PBJointPreStep(joint, inv_dt);
  }

  // Perform iterations
  for(int i = 0; i < world->iterations; i++) {
    for(int j = 0; j < world->arbiters->count; j++) {
      PBArbiter* arbiter = (PBArbiter*)PBArrayGetItem(world->arbiters, j);
      PBArbiterApplyImpulse(arbiter);
    }

    for(int j = 0; j < world->joints->count; j++) {
      PBJoint* joint = PBWorldGetJoint(world, j);
      PBJointApplyImpulse(joint);
    }
  }

  // Integrate Velocities
  for(int i = 0; i < world->bodies->count; i++) {
    PBBody* b = PBWorldGetBody(world, i);
    
    b->position = PBVec2Add(b->position, PBVec2MultF(b->velocity, dt));
    b->rotation += dt * b->angularVelocity;

    b->force.x = 0.0f;
    b->force.y = 0.0f;
    b->torque = 0.0f;
  }
}

void PBWorldBroadphase(PBWorld* world) {
  // O(n^2) broad-phase
  for(int i = 0; i < world->bodies->count; i++) {
    PBBody* bi = PBWorldGetBody(world, i);
    
    for(int j = i + 1; j < world->bodies->count; j++) {
      PBBody* bj = PBWorldGetBody(world, j);
  
      if(bi->invMass == 0.0f && bj->invMass == 0.0f) {
        continue;
      }

      PBArbiter* new_arbiter = PBArbiterCreate(bi, bj);
      int existing_arbiter_i = PBWorldFindArbiter(world, bi, bj);
      
      if(new_arbiter->numContacts > 0) {
        if(existing_arbiter_i == -1) {
          PBArrayAppendItem(world->arbiters, new_arbiter);
        }
        else {
          PBArbiter* arb = (PBArbiter*)PBArrayGetItem(world->arbiters, existing_arbiter_i);
          PBArbiterUpdate(arb, new_arbiter->contacts, new_arbiter->numContacts);
        }
      }
      else {
        if(existing_arbiter_i != -1) {
          PBArrayRemoveItemAt(world->arbiters, existing_arbiter_i);
        }
      }
      
      PBArbiterFree(new_arbiter);
    }
  }
}
