#ifndef MATH_CL
#define MATH_CL

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4 0.785398163397448309616
#endif

#ifndef M_1_PI
#define M_1_PI 0.318309886183790671538
#endif

#ifndef M_2_PI
#define M_2_PI 0.636619772367581343076
#endif

float3 matrixMul3(const float3 *matrix, float3 vector) {
  return matrix[0] * vector.x + matrix[1] * vector.y + matrix[2] * vector.z;
}

float4 matrixMul4(const float4 *matrix, float4 vector) {
  return matrix[0] * vector.x + matrix[1] * vector.y + matrix[2] * vector.z + matrix[3] * vector.w;
}

void orthonormalBasis(float3 n, float3 *t, float3 *b) {
  *t = fabs(n.x) > fabs(n.y) ? (float3)(n.z, 0.0f, -n.x) : (float3)(0.0f, -n.z, n.y);
  *t = normalize(*t);
  *b = cross(n, *t);
}

#endif