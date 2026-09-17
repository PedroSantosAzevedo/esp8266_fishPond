#include "food.h"
#include "fish.h"
#include "flower.h"
#include "config.h"
#include <Arduino.h>

static bool tooCloseToFlowers(const Vec2 &p, Flower flowers[], int flowerCount) {
  for (int i = 0; i < flowerCount; i++) {
    if ((p - flowers[i].pos).length() < FLOWER_AVOID_RADIUS) return true;
  }
  return false;
}

static void spawnFood(Food &food, Flower flowers[], int flowerCount) {
  Vec2 p;
  int attempts = 0;
  do {
    p = Vec2(randomFloat(BOUNDARY_MARGIN, SCREEN_WIDTH - BOUNDARY_MARGIN),
              randomFloat(BOUNDARY_MARGIN, SCREEN_HEIGHT - BOUNDARY_MARGIN));
    attempts++;
  } while (tooCloseToFlowers(p, flowers, flowerCount) && attempts < 20);
  food.pos = p;
  food.active = true;
}

void updateFood(Food foods[], int foodCount, Fish fishes[], int fishCount,
                 Flower flowers[], int flowerCount) {
  for (int i = 0; i < foodCount; i++) {
    Food &food = foods[i];
    if (food.active) {
      for (int j = 0; j < fishCount; j++) {
        if (fishes[j].state == CHASE &&
            (fishes[j].pos - food.pos).length() < EAT_DISTANCE) {
          food.active = false;
          food.nextSpawnAt = millis() + random(FOOD_MIN_RESPAWN_MS, FOOD_MAX_RESPAWN_MS);
          break;
        }
      }
    } else if (millis() >= food.nextSpawnAt) {
      spawnFood(food, flowers, flowerCount);
    }
  }
}
