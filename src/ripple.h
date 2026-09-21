#pragma once
#include "vec2.h"

struct Ripple {
  Vec2 pos;
  bool active = false;
  int dotFramesLeft = 0;   // > 0 while still in the initial "dot" phase
  float radius = 0;        // ring radius once the dot phase is over
  float maxRadius = 0;     // radius at which the ring has fully faded
};

// Advances live ripples and, if ambientEnabled, spawns random ones every so often.
void updateRipples(Ripple ripples[], int count, bool ambientEnabled);

// Starts a ripple in a free slot. Returns false if all slots are busy.
bool spawnRipple(Ripple ripples[], int count, Vec2 pos, int dotFrames, float maxRadius);
