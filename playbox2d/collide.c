#include "platform.h"
#include "arbiter.h"

// Box vertex and edge numbering:
//
//        ^ y
//        |
//        e1
//   v2 ------ v1
//    |        |
// e2 |        | e4  --> x
//    |        |
//   v3 ------ v4
//        e3


typedef enum {
  FACE_A_X,
  FACE_A_Y,
  FACE_B_X,
  FACE_B_Y
} PBAxis;

typedef enum {
  NO_EDGE = 0,
  EDGE1,
  EDGE2,
  EDGE3,
  EDGE4
} PBEdgeNumbers;

typedef struct {
  // ClipVertex() { fp.value = 0; }
  PBVec2 v;
  PBFeaturePair fp;
} PBClipVertex;

static int PBClipSegmentToLine(PBClipVertex vOut[2], PBClipVertex vIn[2], const PBVec2 normal, float offset, char clipEdge) {
  // Start with no output points
  int numOut = 0;

  // Calculate the distance of end points to the line
  float distance0 = PBVec2Dot(normal, vIn[0].v) - offset;
  float distance1 = PBVec2Dot(normal, vIn[1].v) - offset;

  // If the points are behind the plane
  if(distance0 <= 0.0f) { vOut[numOut++] = vIn[0]; }
  if(distance1 <= 0.0f) { vOut[numOut++] = vIn[1]; }

  // If the points are on different sides of the plane
  if(distance0 * distance1 < 0.0f) {
    // Find intersection point of edge and plane
    float interp = distance0 / (distance0 - distance1);
    vOut[numOut].v = PBVec2Add(vIn[0].v, PBVec2MultF(PBVec2Sub(vIn[1].v, vIn[0].v), interp));
    if(distance0 > 0.0f) {
      vOut[numOut].fp = vIn[0].fp;
      vOut[numOut].fp.e.inEdge1 = clipEdge;
      vOut[numOut].fp.e.inEdge2 = NO_EDGE;
    }
    else {
      vOut[numOut].fp = vIn[1].fp;
      vOut[numOut].fp.e.outEdge1 = clipEdge;
      vOut[numOut].fp.e.outEdge2 = NO_EDGE;
    }
    numOut++;
  }

  return numOut;
}

static void ComputeIncidentEdge(PBClipVertex c[2], const PBVec2 h, const PBVec2 pos, const PBMat22 Rot, const PBVec2 normal) {
  // The normal is from the reference box. Convert it
  // to the incident box's frame and flip sign.
  PBMat22 RotT = PBMat22Transpose(Rot);
  PBVec2 n = PBVec2Invert(PBMat22MultVec(RotT, normal));
  PBVec2 nAbs = PBVec2Abs(n);

  if(nAbs.x > nAbs.y) {
    if(PBSign(n.x) > 0.0f) {
      PBVec2Set(&c[0].v, h.x, -h.y);
      c[0].fp.e.inEdge2 = EDGE3;
      c[0].fp.e.outEdge2 = EDGE4;

      PBVec2Set(&c[1].v, h.x, h.y);
      c[1].fp.e.inEdge2 = EDGE4;
      c[1].fp.e.outEdge2 = EDGE1;
    }
    else {
      PBVec2Set(&c[0].v, -h.x, h.y);
      c[0].fp.e.inEdge2 = EDGE1;
      c[0].fp.e.outEdge2 = EDGE2;

      PBVec2Set(&c[1].v, -h.x, -h.y);
      c[1].fp.e.inEdge2 = EDGE2;
      c[1].fp.e.outEdge2 = EDGE3;
    }
  }
  else {
    if(PBSign(n.y) > 0.0f) {
      PBVec2Set(&c[0].v, h.x, h.y);
      c[0].fp.e.inEdge2 = EDGE4;
      c[0].fp.e.outEdge2 = EDGE1;
      
      PBVec2Set(&c[1].v, -h.x, h.y);
      c[1].fp.e.inEdge2 = EDGE1;
      c[1].fp.e.outEdge2 = EDGE2;
    }
    else {
      PBVec2Set(&c[0].v, -h.x, -h.y);
      c[0].fp.e.inEdge2 = EDGE2;
      c[0].fp.e.outEdge2 = EDGE3;

      PBVec2Set(&c[1].v, h.x, -h.y);
      c[1].fp.e.inEdge2 = EDGE3;
      c[1].fp.e.outEdge2 = EDGE4;
    }
  }

  c[0].v = PBVec2Add(pos, PBMat22MultVec(Rot, c[0].v));
  c[1].v = PBVec2Add(pos, PBMat22MultVec(Rot, c[1].v));
}

int PBCollide(PBContact* contacts, PBBody* bodyA, PBBody* bodyB) {
  // Setup
  PBVec2 hA = PBVec2MultF(bodyA->width, 0.5f);
  PBVec2 hB = PBVec2MultF(bodyB->width, 0.5f);

  PBVec2 posA = bodyA->position;
  PBVec2 posB = bodyB->position;

  PBMat22 RotA = PBMat22MakeWithAngle(bodyA->rotation);
  PBMat22 RotB = PBMat22MakeWithAngle(bodyB->rotation);

  PBMat22 RotAT = PBMat22Transpose(RotA);
  PBMat22 RotBT = PBMat22Transpose(RotB);

  PBVec2 dp = PBVec2Sub(posB, posA);
  PBVec2 dA = PBMat22MultVec(RotAT, dp);
  PBVec2 dB = PBMat22MultVec(RotBT, dp);

  PBMat22 C = PBMat22Mult(RotAT, RotB);
  PBMat22 absC = PBMat22Abs(C);
  PBMat22 absCT = PBMat22Transpose(absC);

  // Box A faces
  PBVec2 faceA = PBVec2Sub(PBVec2Sub(PBVec2Abs(dA), hA), PBMat22MultVec(absC, hB));
  if(faceA.x > 0.0f || faceA.y > 0.0f)
    return 0;

  // Box B faces
  PBVec2 faceB = PBVec2Sub(PBVec2Sub(PBVec2Abs(dB), PBMat22MultVec(absCT, hA)), hB);
  if(faceB.x > 0.0f || faceB.y > 0.0f)
    return 0;

  // Find best axis
  PBAxis axis;
  float separation;
  PBVec2 normal;

  // Box A faces
  axis = FACE_A_X;
  separation = faceA.x;
  normal = dA.x > 0.0f ? RotA.col1 : PBVec2Invert(RotA.col1);

  const float relativeTol = 0.95f;
  const float absoluteTol = 0.01f;

  if(faceA.y > relativeTol * separation + absoluteTol * hA.y) {
    axis = FACE_A_Y;
    separation = faceA.y;
    normal = dA.y > 0.0f ? RotA.col2 : PBVec2Invert(RotA.col2);
  }

  // Box B faces
  if(faceB.x > relativeTol * separation + absoluteTol * hB.x) {
    axis = FACE_B_X;
    separation = faceB.x;
    normal = dB.x > 0.0f ? RotB.col1 : PBVec2Invert(RotB.col1);
  }

  if(faceB.y > relativeTol * separation + absoluteTol * hB.y) {
    axis = FACE_B_Y;
    separation = faceB.y;
    normal = dB.y > 0.0f ? RotB.col2 : PBVec2Invert(RotB.col2);
  }

  // Setup clipping plane data based on the separating axis
  PBVec2 frontNormal, sideNormal;
  PBClipVertex incidentEdge[2];
  float front, negSide, posSide;
  char negEdge, posEdge;

  memset(incidentEdge, 0, sizeof(incidentEdge));
  
  // Compute the clipping lines and the line segment to be clipped.
  switch(axis) {
  case FACE_A_X: {
      frontNormal = normal;
      front = PBVec2Dot(posA, frontNormal) + hA.x;
      sideNormal = RotA.col2;
      float side = PBVec2Dot(posA, sideNormal);
      negSide = -side + hA.y;
      posSide =  side + hA.y;
      negEdge = EDGE3;
      posEdge = EDGE1;
      ComputeIncidentEdge(incidentEdge, hB, posB, RotB, frontNormal);
    }
    break;

  case FACE_A_Y: {
      frontNormal = normal;
      front = PBVec2Dot(posA, frontNormal) + hA.y;
      sideNormal = RotA.col1;
      float side = PBVec2Dot(posA, sideNormal);
      negSide = -side + hA.x;
      posSide =  side + hA.x;
      negEdge = EDGE2;
      posEdge = EDGE4;
      ComputeIncidentEdge(incidentEdge, hB, posB, RotB, frontNormal);
    }
    break;

  case FACE_B_X: {
      frontNormal = PBVec2Invert(normal);
      front = PBVec2Dot(posB, frontNormal) + hB.x;
      sideNormal = RotB.col2;
      float side = PBVec2Dot(posB, sideNormal);
      negSide = -side + hB.y;
      posSide =  side + hB.y;
      negEdge = EDGE3;
      posEdge = EDGE1;
      ComputeIncidentEdge(incidentEdge, hA, posA, RotA, frontNormal);
    }
    break;

  case FACE_B_Y: {
      frontNormal = PBVec2Invert(normal);
      front = PBVec2Dot(posB, frontNormal) + hB.y;
      sideNormal = RotB.col1;
      float side = PBVec2Dot(posB, sideNormal);
      negSide = -side + hB.x;
      posSide =  side + hB.x;
      negEdge = EDGE2;
      posEdge = EDGE4;
      ComputeIncidentEdge(incidentEdge, hA, posA, RotA, frontNormal);
    }
    break;
  }

  // clip other face with 5 box planes (1 face plane, 4 edge planes)

  PBClipVertex clipPoints1[2];
  PBClipVertex clipPoints2[2];
  int np;
  
  memset(clipPoints1, 0, sizeof(clipPoints1));
  memset(clipPoints2, 0, sizeof(clipPoints2));

  // Clip to box side 1
  np = PBClipSegmentToLine(clipPoints1, incidentEdge, PBVec2Invert(sideNormal), negSide, negEdge);

  if(np < 2) {
    return 0;
  }

  // Clip to negative box side 1
  np = PBClipSegmentToLine(clipPoints2, clipPoints1, sideNormal, posSide, posEdge);

  if(np < 2) {
    return 0;
  }

  // Now clipPoints2 contains the clipping points.
  // Due to roundoff, it is possible that clipping removes all points.

  int numContacts = 0;
  for(int i = 0; i < 2; ++i) {
    separation = PBVec2Dot(frontNormal, clipPoints2[i].v) - front;

    if(separation <= 0) {
      contacts[numContacts].separation = separation;
      contacts[numContacts].normal = normal;
      // slide contact point onto reference face (easy to cull)
      contacts[numContacts].position = PBVec2Sub(clipPoints2[i].v, PBVec2MultF(frontNormal, separation));
      contacts[numContacts].feature = clipPoints2[i].fp;
      if(axis == FACE_B_X || axis == FACE_B_Y) {
        PBFeaturePair fp = contacts[numContacts].feature;
        char tmp = fp.e.inEdge2;
        fp.e.inEdge2 = fp.e.inEdge1;
        fp.e.inEdge1 = fp.e.inEdge2;
        tmp = fp.e.outEdge2;
        fp.e.outEdge2 = fp.e.outEdge1;
        fp.e.outEdge1 = fp.e.outEdge2;
      }
      numContacts++;
    }
  }

  return numContacts;
}
