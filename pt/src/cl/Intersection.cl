#ifndef INTERSECTION_CL
#define INTERSECTION_CL

struct Intersection {
  float t;
  float u;
  float v;
  uint triangle;
};

typedef struct Intersection Intersection;

#endif