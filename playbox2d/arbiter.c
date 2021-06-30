#include "platform.h"
#include "arbiter.h"

PBArbiter* PBArbiterCreate(PBBody* b1, PBBody* b2) {
  PBArbiter* arbiter = pb_alloc(sizeof(PBArbiter));
  memset(arbiter, 0, sizeof(PBArbiter));
    
    if(b1 == NULL || b2 == NULL) {
        pb_log("playbox: PBArbiter: attempting to create arbiter with NULL body");
    }
  
  if(b1 < b2) {
    arbiter->body1 = b1;
    arbiter->body2 = b2;
  }
  else {
    arbiter->body1 = b2;
    arbiter->body2 = b1;
  }
  
  arbiter->numContacts = PBCollide(arbiter->contacts, arbiter->body1, arbiter->body2);
  arbiter->friction = sqrtf(arbiter->body1->friction * arbiter->body2->friction);
  
  return arbiter;
}

void PBArbiterFree(PBArbiter* arbiter) {
  pb_free(arbiter);
}

void PBArbiterUpdate(PBArbiter* arbiter, PBContact* newContacts, int numNewContacts) {
  PBContact mergedContacts[2];
  
  for(int i = 0; i < numNewContacts; i++) {
    PBContact* cNew = newContacts + i;
    int k = -1;
    for(int j = 0; j < arbiter->numContacts; ++j) {
      PBContact* cOld = arbiter->contacts + j;
      if(cNew->feature.value == cOld->feature.value) {
        k = j;
        break;
      }
    }
  
    if(k > -1) {
      PBContact* cOld = arbiter->contacts + k;
      PBContact* c = mergedContacts + i;
      *c = *cNew;
      if(PBWarmStarting) {
        c->Pn = cOld->Pn;
        c->Pt = cOld->Pt;
        c->Pnb = cOld->Pnb;
      }
      else {
        c->Pn = 0.0f;
        c->Pt = 0.0f;
        c->Pnb = 0.0f;
      }
    }
    else {
      mergedContacts[i] = newContacts[i];
    }
  }
  
  for(int i = 0; i < numNewContacts; ++i) {
    arbiter->contacts[i] = mergedContacts[i];
  }
  
  arbiter->numContacts = numNewContacts;
}

void PBArbiterPreStep(PBArbiter* arbiter, float inv_dt) {
  const float k_allowedPenetration = 0.01f;
  float k_biasFactor = PBPositionCorrection ? 0.2f : 0.0f;

  for(int i = 0; i < arbiter->numContacts; i++) {
    PBContact* c = arbiter->contacts + i;

    PBVec2 r1 = PBVec2Sub(c->position, arbiter->body1->position);
    PBVec2 r2 = PBVec2Sub(c->position, arbiter->body2->position);

    // Precompute normal mass, tangent mass, and bias.
    float rn1 = PBVec2Dot(r1, c->normal);
    float rn2 = PBVec2Dot(r2, c->normal);
    float kNormal = arbiter->body1->invMass + arbiter->body2->invMass;
    kNormal += arbiter->body1->invI * (PBVec2Dot(r1, r1) - rn1 * rn1) + arbiter->body2->invI * (PBVec2Dot(r2, r2) - rn2 * rn2);
    c->massNormal = 1.0f / kNormal;

    PBVec2 tangent = PBVec2CrossF(c->normal, 1.0f);
    float rt1 = PBVec2Dot(r1, tangent);
    float rt2 = PBVec2Dot(r2, tangent);
    float kTangent = arbiter->body1->invMass + arbiter->body2->invMass;
    kTangent += arbiter->body1->invI * (PBVec2Dot(r1, r1) - rt1 * rt1) + arbiter->body2->invI * (PBVec2Dot(r2, r2) - rt2 * rt2);
    c->massTangent = 1.0f /  kTangent;

    c->bias = -k_biasFactor * inv_dt * PBMin(0.0f, c->separation + k_allowedPenetration);

    if(PBAccumulateImpulses) {
      // Apply normal + friction impulse
      PBVec2 P = PBVec2Add(PBVec2MultF(c->normal, c->Pn), PBVec2MultF(tangent, c->Pt));

      arbiter->body1->velocity = PBVec2Sub(arbiter->body1->velocity, PBVec2MultF(P, arbiter->body1->invMass));
      arbiter->body1->angularVelocity -= arbiter->body1->invI * PBVec2Cross(r1, P);

      arbiter->body2->velocity = PBVec2Add(arbiter->body2->velocity, PBVec2MultF(P, arbiter->body2->invMass));
      arbiter->body2->angularVelocity += arbiter->body2->invI * PBVec2Cross(r2, P);
    }
  }
}

void PBArbiterApplyImpulse(PBArbiter* arbiter) {
  PBBody* b1 = arbiter->body1;
  PBBody* b2 = arbiter->body2;

  for(int i = 0; i < arbiter->numContacts; ++i) {
    PBContact* c = arbiter->contacts + i;
    c->r1 = PBVec2Sub(c->position, b1->position);
    c->r2 = PBVec2Sub(c->position, b2->position);

    // Relative velocity at contact
    PBVec2 dv = PBVec2Sub(PBVec2Sub(PBVec2Add(b2->velocity, PBVec2FCross(b2->angularVelocity, c->r2)), b1->velocity), PBVec2FCross(b1->angularVelocity, c->r1));

    // Compute normal impulse
    float vn = PBVec2Dot(dv, c->normal);

    float dPn = c->massNormal * (-vn + c->bias);

    if(PBAccumulateImpulses) {
      // Clamp the accumulated impulse
      float Pn0 = c->Pn;
      c->Pn = PBMax(Pn0 + dPn, 0.0f);
      dPn = c->Pn - Pn0;
    }
    else {
      dPn = PBMax(dPn, 0.0f);
    }

    // Apply contact impulse
    PBVec2 Pn = PBVec2MultF(c->normal, dPn);

    b1->velocity = PBVec2Sub(b1->velocity, PBVec2MultF(Pn, b1->invMass));
    b1->angularVelocity -= b1->invI * PBVec2Cross(c->r1, Pn);

    b2->velocity = PBVec2Add(b2->velocity, PBVec2MultF(Pn, b2->invMass));
    b2->angularVelocity += b2->invI * PBVec2Cross(c->r2, Pn);

    // Relative velocity at contact
    dv = PBVec2Sub(PBVec2Sub(PBVec2Add(b2->velocity, PBVec2FCross(b2->angularVelocity, c->r2)), b1->velocity), PBVec2FCross(b1->angularVelocity, c->r1));

    PBVec2 tangent = PBVec2CrossF(c->normal, 1.0f);
    float vt = PBVec2Dot(dv, tangent);
    float dPt = c->massTangent * (-vt);

    if(PBAccumulateImpulses) {
      // Compute friction impulse
      float maxPt = arbiter->friction * c->Pn;

      // Clamp friction
      float oldTangentImpulse = c->Pt;
      c->Pt = PBClamp(oldTangentImpulse + dPt, -maxPt, maxPt);
      dPt = c->Pt - oldTangentImpulse;
    }
    else
    {
      float maxPt = arbiter->friction * dPn;
      dPt = PBClamp(dPt, -maxPt, maxPt);
    }

    // Apply contact impulse
    PBVec2 Pt = PBVec2MultF(tangent, dPt);

    b1->velocity = PBVec2Sub(b1->velocity, PBVec2MultF(Pt, b1->invMass));
    b1->angularVelocity -= b1->invI * PBVec2Cross(c->r1, Pt);

    b2->velocity = PBVec2Add(b2->velocity, PBVec2MultF(Pt, b2->invMass));
    b2->angularVelocity += b2->invI * PBVec2Cross(c->r2, Pt);
  }
}
