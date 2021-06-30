#include <math.h>
#include <float.h>
#include <assert.h>
#include <stdlib.h>
#include "maths.h"

const float pb_pi = 3.14159265358979323846264f;

inline PBVec2 PBVec2Make(float x, float y) {
  return (PBVec2){.x = x, .y = y};
}

inline PBVec2 PBVec2MakeEmpty(void) {
  return (PBVec2){.x = 0.0f, .y = 0.0f};
}

inline void PBVec2Set(PBVec2* v, float x, float y) {
  v->x = x;
  v->y = y;
}

inline PBVec2 PBVec2Invert(const PBVec2 v) {
  return (PBVec2){.x = -(v.x), .y = -(v.y)};
}

inline PBVec2 PBVec2Add(const PBVec2 v1, const PBVec2 v2) {
  return (PBVec2){.x = (v1.x + v2.x), .y = (v1.y + v2.y)};
}

inline PBVec2 PBVec2Sub(const PBVec2 v1, const PBVec2 v2) {
  return (PBVec2){.x = (v1.x - v2.x), .y = (v1.y - v2.y)};
}

inline PBVec2 PBVec2Mult(const PBVec2 v1, const PBVec2 v2) {
  return (PBVec2){.x = (v1.x * v2.x), .y = (v1.y * v2.y)};
}

inline PBVec2 PBVec2MultF(const PBVec2 v1, float a) {
  return (PBVec2){.x = (v1.x * a), .y = (v1.y * a)};
}

inline float PBVec2GetLength(const PBVec2 v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}

inline float PBVec2Dot(PBVec2 a, PBVec2 b) {
  return a.x * b.x + a.y * b.y;
}

inline float PBVec2Cross(PBVec2 a, PBVec2 b) {
  return a.x * b.y - a.y * b.x;
}

inline PBVec2 PBVec2CrossF(PBVec2 v, float s) {
  return (PBVec2){.x = s * v.y, .y = -s * v.x};
}

inline PBVec2 PBVec2FCross(float s, PBVec2 v) {
  return (PBVec2){.x = -s * v.y, .y = s * v.x};
}





inline PBMat22 PBMat22Make(PBVec2 v1, PBVec2 v2) {
  return (PBMat22){ .col1 = v1, .col2 = v2 };
}

inline PBMat22 PBMat22MakeWithAngle(float angle) {
  float c = cosf(angle), s = sinf(angle);
  return (PBMat22){ .col1.x = c, .col1.y = s, .col2.x = -s, .col2.y = c };
}

inline PBMat22 PBMat22MakeEmpty(void) {
  return (PBMat22){ .col1 = PBVec2MakeEmpty(), .col2 = PBVec2MakeEmpty() };
}

inline PBMat22 PBMat22Transpose(PBMat22 m1) {
  return PBMat22Make(
    PBVec2Make(m1.col1.x, m1.col2.x),
    PBVec2Make(m1.col1.y, m1.col2.y)
  );
}

inline PBMat22 PBMat22Invert(PBMat22 m1) {
  float a = m1.col1.x, b = m1.col2.x, c = m1.col1.y, d = m1.col2.y;
  float det = a * d - b * c;
  det = 1.0f / det;
  
  return PBMat22Make(
    PBVec2Make(det * d, -det * c),
    PBVec2Make(-det * b, det * a)
  );
}

inline PBMat22 PBMat22Mult(PBMat22 m1, PBMat22 m2) {
  return PBMat22Make(PBMat22MultVec(m1, m2.col1), PBMat22MultVec(m1, m2.col2));
}

inline PBVec2 PBMat22MultVec(PBMat22 m, PBVec2 v) {
  return PBVec2Make(m.col1.x * v.x + m.col2.x * v.y, m.col1.y * v.x + m.col2.y * v.y);
}

inline PBMat22 PBMat22Add(PBMat22 m1, PBMat22 m2) {
  return PBMat22Make(PBVec2Add(m1.col1, m2.col1), PBVec2Add(m1.col2, m2.col2));
}



inline float PBAbs(float a) {
  return a > 0.0f ? a : -a;
}

inline PBVec2 PBVec2Abs(PBVec2 v1) {
  return PBVec2Make(fabsf(v1.x), fabsf(v1.y));
}

inline PBMat22 PBMat22Abs(PBMat22 m1) {
  return PBMat22Make(PBVec2Abs(m1.col1), PBVec2Abs(m1.col2));
}

inline float PBSign(float a) {
  return a < 0.0f ? -1.0f : 1.0f;
}

inline float PBMin(float a, float b) {
  return a < b ? a : b;
}

inline float PBMax(float a, float b) {
  return a > b ? a : b;
}

inline float PBClamp(float a, float low, float high) {
  return PBMax(low, PBMin(a, high));
}

inline void PBSwap(void** a, void** b) {
  void* tmp = *a;
  *a = *b;
  *b = tmp;
}

