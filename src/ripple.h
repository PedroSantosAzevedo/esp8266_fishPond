#pragma once
#include "vec2.h"

struct Ripple {
  Vec2 pos;
  bool active = false;
  int dotFramesLeft = 0;   // > 0 while still in the initial "dot" phase
  float radius = 0;        // ring radius once the dot phase is over
};

// Spawns ripples at random spots every so often and advances live ones.
void updateRipples(Ripple ripples[], int count);
