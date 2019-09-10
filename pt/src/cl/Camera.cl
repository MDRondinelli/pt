#ifndef CAMERA_CL
#define CAMERA_CL

struct Camera {
  float4 transform[4];
  float fov;
  float aspect;
};

typedef struct Camera Camera;

#endif