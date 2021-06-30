#ifndef PLAYBOX_MATHS_H
#define PLAYBOX_MATHS_H

#include <math.h>
#include <float.h>
#include <stdlib.h>

extern const float pb_pi;

typedef struct {
  float x, y;
} PBVec2;

extern PBVec2 PBVec2Make(float x, float y);
extern PBVec2 PBVec2MakeEmpty(void);
extern void PBVec2Set(PBVec2* v, float x, float y);
extern PBVec2 PBVec2Invert(const PBVec2 v);
extern PBVec2 PBVec2Add(const PBVec2 v1, const PBVec2 v2);
extern float PBVec2AddF(const PBVec2 v1, float a);
extern PBVec2 PBVec2Sub(const PBVec2 v1, const PBVec2 v2);
extern PBVec2 PBVec2Mult(const PBVec2 v1, const PBVec2 v2);
extern PBVec2 PBVec2MultF(const PBVec2 v1, float a);
extern float PBVec2GetLength(const PBVec2 v1);
extern float PBVec2Dot(PBVec2 v1, PBVec2 v2);
extern float PBVec2Cross(PBVec2 v1, PBVec2 v2);
extern PBVec2 PBVec2CrossF(PBVec2 v1, float s);
extern PBVec2 PBVec2FCross(float s, PBVec2 v1);

typedef struct {
  PBVec2 col1;
  PBVec2 col2;
} PBMat22;

extern PBMat22 PBMat22Make(PBVec2 v1, PBVec2 v2);
extern PBMat22 PBMat22MakeWithAngle(float angle);
extern PBMat22 PBMat22MakeEmpty(void);
extern PBMat22 PBMat22Transpose(PBMat22 m);
extern PBMat22 PBMat22Invert(PBMat22 m);
extern PBMat22 PBMat22Mult(PBMat22 m1, PBMat22 m2);
extern PBVec2 PBMat22MultVec(PBMat22 m1, PBVec2 v1);
extern PBMat22 PBMat22Add(PBMat22 m1, PBMat22 m2);

extern float PBAbs(float a);
extern PBVec2 PBVec2Abs(PBVec2 v1);
extern PBMat22 PBMat22Abs(PBMat22 m1);

extern float PBSign(float a);
extern float PBMin(float a, float b);
extern float PBMax(float a, float b);
extern float PBClamp(float a, float low, float high);

extern void PBSwap(void** a, void** b);

#endif