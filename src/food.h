#pragma once
#include "vec2.h"

// Forward declarations to avoid circular includes with fish.h/leaf.h.
struct Fish;
struct Leaf;

// A single pixel of food that appears at a random point in the lake.
struct Food {
  Vec2 pos;
  bool active = false;
  unsigned long nextSpawnAt = 0;
};

// Handles eating (when a chasing fish reaches active food) and respawning
// (after a random delay, at a random point clear of leaves).
void updateFood(Food foods[], int foodCount,
                 Fish fishes[], int fishCount,
                 Leaf leaves[], int leafCount);
