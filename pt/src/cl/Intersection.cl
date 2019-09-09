#ifndef INTERSECTION_CL
#define INTERSECTION_CL

struct Intersection {
  float t;
  float3 position;
  float3 normal;
  uint bxdf;
};

typedef struct Intersection Intersection;

#endif