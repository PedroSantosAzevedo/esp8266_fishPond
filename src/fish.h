#pragma once
#include "vec2.h"
#include "config.h"

// Forward declarations to avoid circular includes with food.h/leaf.h.
struct Leaf;
struct Food;

enum FishState { WANDER, CHASE };

struct Fish {
  Vec2 pos;
  float heading = 0;         // current direction of travel, radians
  float wanderHeading = 0;   // persistent random-walk heading used while wandering
  float wigglePhase = 0;     // tail animation phase
  FishState state = WANDER;
  int8_t targetFood = -1;    // index into the foods[] array being chased

  // Recent position history, most recent first (trail[0] == pos after the
  // frame's move). The body's spine is sampled from this trail so it curves
  // naturally through turns. See config.h for the sampling constants.
  Vec2 trail[TRAIL_LEN];
};

void initFish(Fish fishes[], int count);

// Steers, turns and moves every fish, applying wander/chase behavior plus
// separation, leaf avoidance and boundary avoidance, and records the new
// position into the fish's trail.
void updateFish(Fish fishes[], int fishCount,
                 Leaf leaves[], int leafCount,
                 Food foods[], int foodCount);

// Assigns CHASE state to whichever fish is currently closest to the active
// food, and WANDER to every other fish. Safe to call every frame.
void assignChaser(Fish fishes[], int fishCount, Food foods[], int foodCount);
