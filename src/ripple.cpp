#include "ripple.h"
#include "config.h"
#include <Arduino.h>

static unsigned long nextSpawnAt = 0;

bool spawnRipple(Ripple ripples[], int count, Vec2 pos, int dotFrames, float maxRadius) {
  for (int i = 0; i < count; i++) {
    if (ripples[i].active) continue;
    ripples[i].pos = pos;
    ripples[i].active = true;
    ripples[i].dotFramesLeft = dotFrames;
    ripples[i].radius = dotFrames > 0 ? 0 : 1;
    ripples[i].maxRadius = maxRadius;
    return true;
  }
  return false;
}

void updateRipples(Ripple ripples[], int count, bool ambientEnabled) {
  unsigned long now = millis();

  if (ambientEnabled && now >= nextSpawnAt) {
    spawnRipple(ripples, count,
                Vec2(randomFloat(0, SCREEN_WIDTH - 1), randomFloat(0, SCREEN_HEIGHT - 1)),
                RIPPLE_DOT_FRAMES, RIPPLE_MAX_RADIUS);
    nextSpawnAt = now + random(RIPPLE_MIN_INTERVAL_MS, RIPPLE_MAX_INTERVAL_MS);
  }

  for (int i = 0; i < count; i++) {
    Ripple &r = ripples[i];
    if (!r.active) continue;
    if (r.dotFramesLeft > 0) {
      r.dotFramesLeft--;
      if (r.dotFramesLeft == 0) r.radius = 1;
      continue;
    }
    r.radius += RIPPLE_GROWTH;
    if (r.radius >= r.maxRadius) r.active = false;
  }
}
