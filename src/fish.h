#pragma once
#include "vec2.h"

// Forward declarations to avoid circular includes with food.h/flower.h.
struct Flower;
struct Food;

enum FishState { WANDER, CHASE };

struct Fish {
  Vec2 pos;
  float heading = 0;         // current direction of travel, radians
  float wanderHeading = 0;   // persistent random-walk heading used while wandering
  float wigglePhase = 0;     // tail animation phase
  FishState state = WANDER;
};

void initFish(Fish fishes[], int count);

// Steers, turns and moves every fish, applying wander/chase behavior plus
// separation, flower avoidance and boundary avoidance.
void updateFish(Fish fishes[], int fishCount,
                 Flower flowers[], int flowerCount,
                 Food foods[], int foodCount);

// Assigns CHASE state to whichever fish is currently closest to the active
// food, and WANDER to every other fish. Safe to call every frame.
void assignChaser(Fish fishes[], int fishCount, Food foods[], int foodCount);
