#include "ripple.h"
#include "config.h"
#include <Arduino.h>

static unsigned long nextSpawnAt = 0;

void updateRipples(Ripple ripples[], int count) {
  unsigned long now = millis();

  if (now >= nextSpawnAt) {
    for (int i = 0; i < count; i++) {
      if (ripples[i].active) continue;
      ripples[i].pos = Vec2(randomFloat(0, SCREEN_WIDTH - 1), randomFloat(0, SCREEN_HEIGHT - 1));
      ripples[i].active = true;
      ripples[i].dotFramesLeft = RIPPLE_DOT_FRAMES;
      ripples[i].radius = 0;
      break;
    }
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
    if (r.radius >= RIPPLE_MAX_RADIUS) r.active = false;
  }
}
