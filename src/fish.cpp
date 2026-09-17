#include "fish.h"
#include "food.h"
#include "leaf.h"
#include "config.h"
#include <Arduino.h>
#include <math.h>
#include <string.h>

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
    for (int j = 0; j < TRAIL_LEN; j++) fishes[i].trail[j] = fishes[i].pos;
  }
}

void assignChaser(Fish fishes[], int fishCount, Food foods[], int foodCount) {
  // Each active food claims its nearest still-unclaimed fish (foods are
  // processed in index order, so with NUM_FOOD < NUM_FISH every food
  // normally gets a chaser). Hysteresis per food avoids handing the chase
  // to a marginally-closer fish every single frame.
  bool claimed[NUM_FISH] = {false};
  int8_t prevTarget[NUM_FISH];
  for (int i = 0; i < fishCount; i++) {
    prevTarget[i] = (fishes[i].state == CHASE) ? fishes[i].targetFood : (int8_t)-1;
    fishes[i].state = WANDER;
    fishes[i].targetFood = -1;
  }

  for (int k = 0; k < foodCount; k++) {
    if (!foods[k].active) continue;

    int bestIdx = -1;
    float bestDist = 1e9f;
    int prevChaser = -1;
    float prevChaserDist = 1e9f;
    for (int i = 0; i < fishCount; i++) {
      if (claimed[i]) continue;
      float d = (fishes[i].pos - foods[k].pos).length();
      if (d < bestDist) {
        bestDist = d;
        bestIdx = i;
      }
      if (prevTarget[i] == k) {
        prevChaser = i;
        prevChaserDist = d;
      }
    }

    int chosen = bestIdx;
    if (prevChaser != -1 && prevChaser != bestIdx &&
        (prevChaserDist - bestDist) < CHASE_SWITCH_MARGIN) {
      chosen = prevChaser;
    }

    if (chosen != -1) {
      fishes[chosen].state = CHASE;
      fishes[chosen].targetFood = k;
      claimed[chosen] = true;
    }
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

void updateFish(Fish fishes[], int fishCount, Leaf leaves[], int leafCount,
                 Food foods[], int foodCount) {
  for (int i = 0; i < fishCount; i++) {
    Fish &f = fishes[i];

    // 1. Desired direction from current behavior state.
    Vec2 desired;
    if (f.state == CHASE) {
      if (f.targetFood >= 0 && f.targetFood < foodCount && foods[f.targetFood].active) {
        desired = (foods[f.targetFood].pos - f.pos).normalized() * CHASE_WEIGHT;
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

    // 3. Leaf avoidance (soft steering).
    if (LEAF_SHOULD_COLIDE) {
      for (int k = 0; k < leafCount; k++) {
        Vec2 diff = f.pos - leaves[k].pos;
        float d = diff.length();
        if (d < LEAF_AVOID_RADIUS && d > 0.001f) {
          steer += diff.normalized() *
                 (LEAF_AVOID_WEIGHT * (LEAF_AVOID_RADIUS - d) / LEAF_AVOID_RADIUS);
        }
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

    // 8. Hard leaf-collision correction: never let a fish's center enter
    //    a leaf's hard radius, even under strong chase steering.
    for (int k = 0; k < leafCount; k++) {
      Vec2 diff = f.pos - leaves[k].pos;
      float d = diff.length();
      if (d < LEAF_HARD_RADIUS && d > 0.001f) {
        f.pos += diff.normalized() * (LEAF_HARD_RADIUS - d);
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

    // 11. Record the final, corrected position into the trail so the body
    //     spine reflects where the fish actually ended up this frame.
    memmove(&f.trail[1], &f.trail[0], sizeof(Vec2) * (TRAIL_LEN - 1));
    f.trail[0] = f.pos;
  }
}
