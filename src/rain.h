#pragma once
#include "vec2.h"
#include "ripple.h"
#include "controls.h"

struct Raindrop {
  Vec2 pos;          // head of the streak
  Vec2 vel;          // px per frame
  int framesLeft = 0;
  bool active = false;
};

// Spawns drops while rain is enabled, moves them, and creates a small ripple
// where each one lands. Drops already in the air finish falling if rain is
// switched off.
void updateRain(Raindrop drops[], int dropCount, Ripple ripples[], int rippleCount,
                const Controls &controls);
