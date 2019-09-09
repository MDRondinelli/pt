#ifndef RANDOM_CL
#define RANDOM_CL

uint mwc64x(__global uint2 *state) {
  enum { A = 4294883355U };
  uint x = state->x;
  uint c = state->y;
  uint res = x ^ c;
  uint hi = mul_hi(x, A);
  x = x * A + c;
  c = hi + (x < c);
  *state = (uint2)(x, c);
  return res;
}

float rand(__global uint2 *state) {
  return (mwc64x(state) & 65535) / 65536.0f;
}

float2 rand2(__global uint2 *state) {
  return (float2)(rand(state), rand(state));
}

float3 rand3(__global uint2 *state) {
  return (float3)(rand(state), rand(state), rand(state));
}

#endif