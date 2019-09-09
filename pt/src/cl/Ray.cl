#ifndef RAY_CL
#define RAY_CL

struct Ray {
  float3 o;
  float3 d;
  float tmin;
  float tmax;
};

typedef struct Ray Ray;

#endif