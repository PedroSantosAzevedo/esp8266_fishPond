#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "vec2.h"
#include "fish.h"
#include "food.h"
#include "flower.h"
#include "render.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Fish fishes[NUM_FISH];
Flower flowers[NUM_FLOWERS];
Food foods[NUM_FOOD];

unsigned long lastFrameAt = 0;

static bool tooCloseToOtherFlowers(const Vec2 &p, int placedCount) {
  for (int i = 0; i < placedCount; i++) {
    if ((p - flowers[i].pos).length() < FLOWER_AVOID_RADIUS * 1.5f) return true;
  }
  return false;
}

static void initFlowers() {
  for (int i = 0; i < NUM_FLOWERS; i++) {
    Vec2 p;
    int attempts = 0;
    do {
      p = Vec2(randomFloat(BOUNDARY_MARGIN + 4, SCREEN_WIDTH - BOUNDARY_MARGIN - 4),
                randomFloat(BOUNDARY_MARGIN + 4, SCREEN_HEIGHT - BOUNDARY_MARGIN - 4));
      attempts++;
    } while (tooCloseToOtherFlowers(p, i) && attempts < 30);
    flowers[i].pos = p;
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

  initFlowers();
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

  updateFood(foods, NUM_FOOD, fishes, NUM_FISH, flowers, NUM_FLOWERS);
  assignChaser(fishes, NUM_FISH, foods, NUM_FOOD);
  updateFish(fishes, NUM_FISH, flowers, NUM_FLOWERS, foods, NUM_FOOD);

  display.clearDisplay();
  for (int i = 0; i < NUM_FLOWERS; i++) drawFlower(display, flowers[i]);
  for (int i = 0; i < NUM_FOOD; i++) drawFood(display, foods[i]);
  for (int i = 0; i < NUM_FISH; i++) drawFish(display, fishes[i]);
  display.display();
}
