#include "fish.h"
#include "food.h"
#include "flower.h"
#include "config.h"
#include <Arduino.h>
#include <math.h>

static float wrapAngle(float a) {
  while (a > PI) a -= 2 * PI;
  while (a < -PI) a += 2 * PI;
  return a;
}

void initFish(Fish fishes[], int count) {
  for (int i = 0; i < count; i++) {
    fishes[i].pos = Vec2(
        randomFloat(BOUNDARY_MARGIN + 4, SCREEN_WIDTH - BOUNDARY_MARGIN - 4),
        randomFloat(BOUNDARY_MARGIN + 4, SCREEN_HEIGHT - BOUNDARY_MARGIN - 4));
    fishes[i].heading = randomFloat(-PI, PI);
    fishes[i].wanderHeading = fishes[i].heading;
    fishes[i].wigglePhase = randomFloat(0, 2 * PI);
    fishes[i].state = WANDER;
  }
}

void assignChaser(Fish fishes[], int fishCount, Food foods[], int foodCount) {
  // Single-food design today, but written to generalize if NUM_FOOD grows.
  int activeFoodIdx = -1;
  for (int f = 0; f < foodCount; f++) {
    if (foods[f].active) {
      activeFoodIdx = f;
      break;
    }
  }

  if (activeFoodIdx == -1) {
    for (int i = 0; i < fishCount; i++) fishes[i].state = WANDER;
    return;
  }

  Vec2 foodPos = foods[activeFoodIdx].pos;
  int bestIdx = -1;
  float bestDist = 1e9f;
  int currentChaser = -1;
  float currentChaserDist = 1e9f;

  for (int i = 0; i < fishCount; i++) {
    float d = (fishes[i].pos - foodPos).length();
    if (d < bestDist) {
      bestDist = d;
      bestIdx = i;
    }
    if (fishes[i].state == CHASE) {
      currentChaser = i;
      currentChaserDist = d;
    }
  }

  // Hysteresis: don't hand the chase to a marginally-closer fish every frame.
  int chosen = bestIdx;
  if (currentChaser != -1 && currentChaser != bestIdx &&
      (currentChaserDist - bestDist) < CHASE_SWITCH_MARGIN) {
    chosen = currentChaser;
  }

  for (int i = 0; i < fishCount; i++) {
    fishes[i].state = (i == chosen) ? CHASE : WANDER;
  }
}

static Vec2 boundaryForce(const Vec2 &pos) {
  Vec2 f(0, 0);
  if (pos.x < BOUNDARY_MARGIN) f.x += (BOUNDARY_MARGIN - pos.x);
  if (pos.x > SCREEN_WIDTH - BOUNDARY_MARGIN)
    f.x -= (pos.x - (SCREEN_WIDTH - BOUNDARY_MARGIN));
  if (pos.y < BOUNDARY_MARGIN) f.y += (BOUNDARY_MARGIN - pos.y);
  if (pos.y > SCREEN_HEIGHT - BOUNDARY_MARGIN)
    f.y -= (pos.y - (SCREEN_HEIGHT - BOUNDARY_MARGIN));
  return f;
}

void updateFish(Fish fishes[], int fishCount, Flower flowers[], int flowerCount,
                 Food foods[], int foodCount) {
  for (int i = 0; i < fishCount; i++) {
    Fish &f = fishes[i];

    // 1. Desired direction from current behavior state.
    Vec2 desired;
    if (f.state == CHASE) {
      int fi = -1;
      for (int k = 0; k < foodCount; k++) {
        if (foods[k].active) {
          fi = k;
          break;
        }
      }
      if (fi >= 0) {
        desired = (foods[fi].pos - f.pos).normalized() * CHASE_WEIGHT;
      } else {
        f.state = WANDER;
      }
    }
    if (f.state == WANDER) {
      f.wanderHeading =
          wrapAngle(f.wanderHeading + randomFloat(-WANDER_JITTER, WANDER_JITTER));
      desired = Vec2(cosf(f.wanderHeading), sinf(f.wanderHeading)) * WANDER_WEIGHT;
    }

    Vec2 steer = desired;

    // 2. Separation from other fish (soft steering, prevents crashing).
    for (int j = 0; j < fishCount; j++) {
      if (j == i) continue;
      Vec2 diff = f.pos - fishes[j].pos;
      float d = diff.length();
      if (d < SEPARATION_DIST && d > 0.001f) {
        steer += diff.normalized() * (SEPARATION_WEIGHT * (SEPARATION_DIST - d) / SEPARATION_DIST);
      }
    }

    // 3. Flower avoidance (soft steering).
    for (int k = 0; k < flowerCount; k++) {
      Vec2 diff = f.pos - flowers[k].pos;
      float d = diff.length();
      if (d < FLOWER_AVOID_RADIUS && d > 0.001f) {
        steer += diff.normalized() *
                 (FLOWER_AVOID_WEIGHT * (FLOWER_AVOID_RADIUS - d) / FLOWER_AVOID_RADIUS);
      }
    }

    // 4. Boundary avoidance.
    steer += boundaryForce(f.pos) * (BOUNDARY_WEIGHT * 0.1f);

    // 5. Turn toward the net steering direction, limited by max turn rate.
    if (steer.length() > 0.001f) {
      float targetAngle = atan2f(steer.y, steer.x);
      float diffAngle = wrapAngle(targetAngle - f.heading);
      if (diffAngle > FISH_MAX_TURN_RATE) diffAngle = FISH_MAX_TURN_RATE;
      if (diffAngle < -FISH_MAX_TURN_RATE) diffAngle = -FISH_MAX_TURN_RATE;
      f.heading = wrapAngle(f.heading + diffAngle);
    }

    // 6. Move forward.
    f.pos.x += cosf(f.heading) * FISH_SPEED;
    f.pos.y += sinf(f.heading) * FISH_SPEED;

    // 7. Hard screen-edge clamp (safety net behind the soft boundary steering).
    if (f.pos.x < 1) f.pos.x = 1;
    if (f.pos.x > SCREEN_WIDTH - 1) f.pos.x = SCREEN_WIDTH - 1;
    if (f.pos.y < 1) f.pos.y = 1;
    if (f.pos.y > SCREEN_HEIGHT - 1) f.pos.y = SCREEN_HEIGHT - 1;

    // 8. Hard flower-collision correction: never let a fish's center enter
    //    a flower's hard radius, even under strong chase steering.
    for (int k = 0; k < flowerCount; k++) {
      Vec2 diff = f.pos - flowers[k].pos;
      float d = diff.length();
      if (d < FLOWER_HARD_RADIUS && d > 0.001f) {
        f.pos += diff.normalized() * (FLOWER_HARD_RADIUS - d);
      }
    }

    // 9. Hard fish-fish collision correction: last-resort anti-overlap.
    for (int j = 0; j < fishCount; j++) {
      if (j == i) continue;
      Vec2 diff = f.pos - fishes[j].pos;
      float d = diff.length();
      if (d < SEPARATION_HARD_DIST && d > 0.001f) {
        f.pos += diff.normalized() * (SEPARATION_HARD_DIST - d) * 0.5f;
      }
    }

    // 10. Advance tail wiggle animation.
    f.wigglePhase += FISH_WIGGLE_SPEED;
    if (f.wigglePhase > 1000.0f) f.wigglePhase -= 1000.0f;
  }
}
