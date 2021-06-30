#include "playbox.h"
#include "maths.h"
#include "platform.h"

static const lua_reg worldClass[];
static const lua_reg bodyClass[];
static const lua_reg jointClass[];

#define CLASSNAME_WORLD "playbox.world"
#define CLASSNAME_BODY "playbox.body"
#define CLASSNAME_JOINT "playbox.joint"

void registerPlaybox(void) {
  const char* err = NULL;
  
  // Register body
  if(!pd->lua->registerClass(CLASSNAME_BODY, bodyClass, NULL, 0, &err)) {
    pb_log("playbox: Failed to register body class. %s", err);
    return;
  }
  
  // Register joint
  if(!pd->lua->registerClass(CLASSNAME_JOINT, jointClass, NULL, 0, &err)) {
    pb_log("playbox: Failed to register joint class. %s", err);
    return;
  }
  
  // Register world
  if(!pd->lua->registerClass(CLASSNAME_WORLD, worldClass, NULL, 0, &err)) {
    pb_log("playbox: Failed to register world class. %s", err);
    return;
  }
}

// UTILITIES

static PBWorld* getWorldArg(int n) { return pd->lua->getArgObject(n, CLASSNAME_WORLD, NULL); }
static PBBody* getBodyArg(int n) { return pd->lua->getArgObject(n, CLASSNAME_BODY, NULL); }
static PBJoint* getJointArg(int n) { return pd->lua->getArgObject(n, CLASSNAME_JOINT, NULL); }

// BODY CLASS

int playbox_body_new(lua_State* L) {
  PBBody* body = PBBodyCreate();
  
  float w = pd->lua->getArgFloat(1);
  float h = pd->lua->getArgFloat(2);
  float m = pd->lua->getArgFloat(3);
  
  if(m == 0.0) {
    m = FLT_MAX;
  }
  
  PBBodySet(body, PBVec2Make(w, h), m);
  
  pd->lua->pushObject(body, CLASSNAME_BODY, 0);
  return 1;
}

int playbox_body_delete(lua_State* L) {
  PBBody* body = getBodyArg(1);
  if(body != NULL) {
    PBBodyFree(body);
  }
    
  return 0;
}

int playbox_body_addForce(lua_State* L) {
  PBBody* body = getBodyArg(1);
  float x = pd->lua->getArgFloat(2);
  float y = pd->lua->getArgFloat(3);
  PBBodyAddForce(body, PBVec2Make(x, y));
  return 0;
}

int playbox_body_setCenter(lua_State* L) {
  PBBody* body = getBodyArg(1);
  body->position.x = pd->lua->getArgFloat(2);
  body->position.y = pd->lua->getArgFloat(3);
  return 0;
}

int playbox_body_setRotation(lua_State* L) {
  PBBody* body = getBodyArg(1);
  body->rotation = pd->lua->getArgFloat(2);
  return 0;
}

int playbox_body_setVelocity(lua_State* L) {
  PBBody* body = getBodyArg(1);
  body->velocity.x = pd->lua->getArgFloat(2);
  body->velocity.y = pd->lua->getArgFloat(3);
  return 0;
}

int playbox_body_setAngularVelocity(lua_State* L) {
  PBBody* body = getBodyArg(1);
  body->angularVelocity = pd->lua->getArgFloat(2);
  return 0;
}

int playbox_body_setForce(lua_State* L) {
  PBBody* body = getBodyArg(1);
  body->force.x = pd->lua->getArgFloat(2);
  body->force.y = pd->lua->getArgFloat(3);
  return 0;
}

int playbox_body_setTorque(lua_State* L) {
  PBBody* body = getBodyArg(1);
  body->torque = pd->lua->getArgFloat(2);
  return 0;
}

int playbox_body_setSize(lua_State* L) {
  PBBody* body = getBodyArg(1);
  body->width.x = pd->lua->getArgFloat(2);
  body->width.y = pd->lua->getArgFloat(3);
  body->AABBHalfSize = PBVec2GetLength(body->width) * 0.5f;
  return 0;
}

int playbox_body_setFriction(lua_State* L) {
  PBBody* body = getBodyArg(1);
  body->friction = pd->lua->getArgFloat(2);
  return 0;
}

int playbox_body_setMass(lua_State* L) {
  PBBody* body = getBodyArg(1);
  body->mass = pd->lua->getArgFloat(2);
  if(body->mass < FLT_MAX) {
    body->invMass = 1.0f / body->mass;
  }
  else {
    body->invMass = 0.0f;
  }
  return 0;
}

int playbox_body_setI(lua_State* L) {
  PBBody* body = getBodyArg(1);
  body->I = pd->lua->getArgFloat(2);
  if(body->I < FLT_MAX) {
    body->invI = 1.0f / body->I;
  }
  else {
    body->invI = 0.0f;
  }
  return 0;
}

int playbox_body_getCenter(lua_State* L) {
  PBBody* body = getBodyArg(1);
  pd->lua->pushFloat(body->position.x);
  pd->lua->pushFloat(body->position.y);
  return 2;
}

int playbox_body_getRotation(lua_State* L) {
  PBBody* body = getBodyArg(1);
  pd->lua->pushFloat(body->rotation);
  return 1;
}

int playbox_body_getSize(lua_State* L) {
  PBBody* body = getBodyArg(1);
  pd->lua->pushFloat(body->width.x);
  pd->lua->pushFloat(body->width.y);
  return 2;
}

int playbox_body_getVelocity(lua_State* L) {
  PBBody* body = getBodyArg(1);
  pd->lua->pushFloat(body->velocity.x);
  pd->lua->pushFloat(body->velocity.y);
  return 2;
}

int playbox_body_getPolygon(lua_State* L) {
  PBBody* body = getBodyArg(1);
  PBWorld* world = body->world;
  
  PBMat22 R = PBMat22MakeWithAngle(body->rotation);
  PBVec2 x = body->position;
  PBVec2 h = PBVec2MultF(body->width, 0.5f);

  PBVec2 v1 = PBVec2Add(x, PBMat22MultVec(R, PBVec2Make(-h.x, -h.y)));
  PBVec2 v2 = PBVec2Add(x, PBMat22MultVec(R, PBVec2Make( h.x, -h.y)));
  PBVec2 v3 = PBVec2Add(x, PBMat22MultVec(R, PBVec2Make( h.x,  h.y)));
  PBVec2 v4 = PBVec2Add(x, PBMat22MultVec(R, PBVec2Make(-h.x,  h.y)));
  
  float scale = 1.0;
  if(world != NULL) {
    scale = world->pixelScale;
  }
  
  pd->lua->pushFloat(v1.x * scale);
  pd->lua->pushFloat(v1.y * scale);
  pd->lua->pushFloat(v2.x * scale);
  pd->lua->pushFloat(v2.y * scale);
  pd->lua->pushFloat(v3.x * scale);
  pd->lua->pushFloat(v3.y * scale);
  pd->lua->pushFloat(v4.x * scale);
  pd->lua->pushFloat(v4.y * scale);
  
  return 8;
}

static const lua_reg bodyClass[] = {
{ "new", playbox_body_new },
{ "__gc", playbox_body_delete },
{ "addForce", playbox_body_addForce },
{ "setCenter", playbox_body_setCenter },
{ "getCenter", playbox_body_getCenter },
{ "setRotation", playbox_body_setRotation },
{ "getRotation", playbox_body_getRotation },
{ "setVelocity", playbox_body_setVelocity },
{ "setVelocity", playbox_body_getVelocity },
{ "setAngularVelocity", playbox_body_setAngularVelocity },
{ "setForce", playbox_body_setForce },
{ "setTorque", playbox_body_setTorque },
{ "setSize", playbox_body_setSize },
{ "getSize", playbox_body_getSize },
{ "setFriction", playbox_body_setFriction },
{ "setMass", playbox_body_setMass },
{ "setI", playbox_body_setI },
{ "getPolygon", playbox_body_getPolygon },
{ NULL, NULL }
};


// JOINT CLASS

int playbox_joint_new(lua_State* L) {
  PBBody* body1 = getBodyArg(1);
  PBBody* body2 = getBodyArg(2);
  float anchor_x = pd->lua->getArgFloat(3);
  float anchor_y = pd->lua->getArgFloat(4);
  
  PBJoint* joint = PBJointCreate(body1, body2, PBVec2Make(anchor_x, anchor_y));
  pd->lua->pushObject(joint, CLASSNAME_JOINT, 0);
  
  return 1;
}

int playbox_joint_delete(lua_State* L) {
  PBJoint* joint = getJointArg(1);
  if(joint != NULL) {
    PBJointFree(joint);
  }
  return 0;
}

int playbox_joint_getPoints(lua_State* L) {
  PBJoint* joint = getJointArg(1);
  PBWorld* world = joint->world;
  float scale = 1.0;
  if(world != NULL) {
    scale = world->pixelScale;
  }
  
  PBBody* b1 = joint->body1;
  PBBody* b2 = joint->body2;

  PBMat22 R1 = PBMat22MakeWithAngle(b1->rotation);
  PBMat22 R2 = PBMat22MakeWithAngle(b2->rotation);

  PBVec2 x1 = b1->position;
  PBVec2 p1 = PBVec2Add(x1, PBMat22MultVec(R1, joint->localAnchor1));

  PBVec2 x2 = b2->position;
  PBVec2 p2 = PBVec2Add(x2, PBMat22MultVec(R2, joint->localAnchor2));
  
  pd->lua->pushFloat(x1.x * scale);
  pd->lua->pushFloat(x1.y * scale);
  pd->lua->pushFloat(p1.x * scale);
  pd->lua->pushFloat(p1.y * scale);
  pd->lua->pushFloat(x2.x * scale);
  pd->lua->pushFloat(x2.y * scale);
  pd->lua->pushFloat(p2.x * scale);
  pd->lua->pushFloat(p2.y * scale);
  
  return 8;
}

int playbox_joint_setSoftness(lua_State* L) {
  PBJoint* joint = getJointArg(1);
  joint->softness = pd->lua->getArgFloat(2);
  return 0;
}

int playbox_joint_setBiasFactor(lua_State* L) {
  PBJoint* joint = getJointArg(1);
  joint->biasFactor = pd->lua->getArgFloat(2);
  return 0;
}

static const lua_reg jointClass[] = {
{ "new", playbox_joint_new },
{ "__gc", playbox_joint_delete },
{ "getPoints", playbox_joint_getPoints },
{ "setSoftness", playbox_joint_setSoftness },
{ "setBiasFactor", playbox_joint_setBiasFactor },
{ NULL, NULL }
};


// WORLD CLASS

int playbox_world_new(lua_State* L) {
  float gravity_x = pd->lua->getArgFloat(1);
  float gravity_y = pd->lua->getArgFloat(2);
  int iterations = pd->lua->getArgInt(3);
  
  PBWorld* world = PBWorldCreate(PBVec2Make(gravity_x, gravity_y), iterations);
  pd->lua->pushObject(world, CLASSNAME_WORLD, 0);
  return 1;
}

int playbox_world_delete(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  if(world != NULL) {
    PBWorldFree(world);
  }
  return 0;
}

int playbox_world_addBody(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  PBBody* body = getBodyArg(2);
  PBWorldAddBody(world, body);
  return 0;
}

int playbox_world_removeBody(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  PBBody* body = getBodyArg(2);
  PBWorldRemoveBody(world, body);
  return 0;
}

int playbox_world_addJoint(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  PBJoint* joint = getJointArg(2);
  PBWorldAddJoint(world, joint);
  return 0;
}

int playbox_world_removeJoint(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  PBJoint* joint = getJointArg(2);
  PBWorldRemoveJoint(world, joint);
  return 0;
}

int playbox_world_clear(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  PBWorldClear(world);
  return 0;
}

int playbox_world_step(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  float dt = pd->lua->getArgFloat(2);
  PBWorldStep(world, dt);
  return 0;
}

int playbox_world_getArbiterCount(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  pd->lua->pushInt(world->arbiters->count);
  return 1;
}

int playbox_world_getArbiterPosition(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  int i = pd->lua->getArgInt(2);
  
  i -= 1;
  PBArbiter* arbiter = (PBArbiter*)PBArrayGetItem(world->arbiters, i);
  
  for(int j = 0; j < arbiter->numContacts; j++) {
    PBVec2 p = arbiter->contacts[j].position;
    pd->lua->pushFloat(p.x);
    pd->lua->pushFloat(p.y);
  }
  
  return arbiter->numContacts * 2;
}

int playbox_world_setPixelScale(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  float scale = pd->lua->getArgFloat(2);
  world->pixelScale = scale;
  return 0;
}

int playbox_world_getNumberOfContacts(lua_State* L) {
  PBWorld* world = getWorldArg(1);
  PBBody* body1 = getBodyArg(2);
  PBBody* body2 = getBodyArg(3);
  int contacts = PBWorldNumberOfContactsBetweenBodies(world, body1, body2);
  pd->lua->pushInt(contacts);
  return 1;
}

static const lua_reg worldClass[] = {
{ "new", playbox_world_new },
{ "__gc", playbox_world_delete },
{ "addBody", playbox_world_addBody },
{ "removeBody", playbox_world_removeBody },
{ "addJoint", playbox_world_addJoint },
{ "removeJoint", playbox_world_removeJoint },
{ "clear", playbox_world_clear },
{ "update", playbox_world_step },
{ "getArbiterCount", playbox_world_getArbiterCount },
{ "getArbiterPosition", playbox_world_getArbiterPosition },
{ "setPixelScale", playbox_world_setPixelScale },
{ "getNumberOfContacts", playbox_world_getNumberOfContacts },
{ NULL, NULL }
};
