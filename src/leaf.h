#pragma once
#include "vec2.h"

// A static lily-pad obstacle in the lake. Position and notch are fixed once
// at startup. The notch is precomputed (rather than stored as an angle) so
// drawing never needs to call sin/cos.
struct Leaf {
  Vec2 pos;
  Vec2 notchA;  // offset from pos to one edge of the notch wedge
  Vec2 notchB;  // offset from pos to the other edge of the notch wedge
};
