#include "rain.h"
#include "config.h"
#include <Arduino.h>

void updateRain(Raindrop drops[], int dropCount, Ripple ripples[], int rippleCount,
                const Controls &controls) {
  if (controls.rainEnabled && random(100) < RAIN_SPAWN_CHANCE_PCT) {
    for (int i = 0; i < dropCount; i++) {
      if (drops[i].active) continue;
      // Pick where the drop lands, then start it far enough back along its
      // path so it arrives there after framesLeft frames.
      Vec2 impact(randomFloat(0, SCREEN_WIDTH - 1), randomFloat(0, SCREEN_HEIGHT - 1));
      Vec2 vel(controls.wind * RAIN_WIND_STEP, RAIN_SPEED);
      int frames = random(RAIN_MIN_FALL_FRAMES, RAIN_MAX_FALL_FRAMES + 1);
      drops[i].vel = vel;
      drops[i].pos = impact - vel * (float)frames;
      drops[i].framesLeft = frames;
      drops[i].active = true;
      break;
    }
  }

  for (int i = 0; i < dropCount; i++) {
    Raindrop &d = drops[i];
    if (!d.active) continue;
    d.pos += d.vel;
    if (--d.framesLeft <= 0) {
      d.active = false;
      spawnRipple(ripples, rippleCount, d.pos, 0, RAIN_RIPPLE_MAX_RADIUS);
    }
  }
}
