#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#include "config.h"
#include "vec2.h"
#include "fish.h"
#include "food.h"
#include "leaf.h"
#include "render.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Fish fishes[NUM_FISH];
Leaf leaves[NUM_LEAVES];
Food foods[NUM_FOOD];

unsigned long lastFrameAt = 0;

static bool tooCloseToOtherLeaves(const Vec2 &p, int placedCount) {
  for (int i = 0; i < placedCount; i++) {
    if ((p - leaves[i].pos).length() < LEAF_AVOID_RADIUS * 1.5f) return true;
  }
  return false;
}

static void initLeaves() {
  for (int i = 0; i < NUM_LEAVES; i++) {
    Vec2 p;
    int attempts = 0;
    do {
      p = Vec2(randomFloat(BOUNDARY_MARGIN + 4, SCREEN_WIDTH - BOUNDARY_MARGIN - 4),
                randomFloat(BOUNDARY_MARGIN + 4, SCREEN_HEIGHT - BOUNDARY_MARGIN - 4));
      attempts++;
    } while (tooCloseToOtherLeaves(p, i) && attempts < 30);
    leaves[i].pos = p;

    // Precompute the notch wedge once so drawing never needs sin/cos.
    float notchAngle = randomFloat(-PI, PI);
    float na1 = notchAngle - LEAF_NOTCH_HALF_ANGLE;
    float na2 = notchAngle + LEAF_NOTCH_HALF_ANGLE;
    float notchLen = LEAF_RADIUS + 1.0f;
    leaves[i].notchA = Vec2(cosf(na1), sinf(na1)) * notchLen;
    leaves[i].notchB = Vec2(cosf(na2), sinf(na2)) * notchLen;
  }
}

void setup() {
  randomSeed(analogRead(A0));
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // Display not found: halt so the failure is obvious rather than
    // silently running with no output.
    for (;;) {
      delay(1000);
    }
  }
  display.clearDisplay();
  display.display();

  initLeaves();
  initFish(fishes, NUM_FISH);
  for (int i = 0; i < NUM_FOOD; i++) {
    foods[i].active = false;
    foods[i].nextSpawnAt = millis() + random(500, 2000);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - lastFrameAt < FRAME_INTERVAL_MS) return;
  lastFrameAt = now;

  updateFood(foods, NUM_FOOD, fishes, NUM_FISH, leaves, NUM_LEAVES);
  assignChaser(fishes, NUM_FISH, foods, NUM_FOOD);
  updateFish(fishes, NUM_FISH, leaves, NUM_LEAVES, foods, NUM_FOOD);

  display.clearDisplay();
  for (int i = 0; i < NUM_LEAVES; i++) drawLeaf(display, leaves[i]);
  for (int i = 0; i < NUM_FOOD; i++) drawFood(display, foods[i]);
  for (int i = 0; i < NUM_FISH; i++) drawFish(display, fishes[i]);
  display.display();
}
