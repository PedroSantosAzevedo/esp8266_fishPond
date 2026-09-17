#include "render.h"
#include "config.h"
#include <math.h>

// Fish are drawn procedurally each frame as a small rotated silhouette
// (tapered body from overlapping circles + a wiggling triangular tail)
// rather than a fixed sprite sheet. This gives smooth rotation to any
// heading angle (not just 8 snapped directions) and a continuous tail
// animation, at negligible CPU cost for 3 fish at ~30fps.

static void rotatePoint(float lx, float ly, float cosA, float sinA, float cx, float cy,
                          int &outX, int &outY) {
  float wx = lx * cosA - ly * sinA;
  float wy = lx * sinA + ly * cosA;
  outX = (int)roundf(cx + wx);
  outY = (int)roundf(cy + wy);
}

void drawFlower(Adafruit_SSD1306 &display, const Flower &flower) {
  int cx = (int)roundf(flower.pos.x);
  int cy = (int)roundf(flower.pos.y);
  int offset = (int)FLOWER_PETAL_OFFSET;
  int r = (int)FLOWER_PETAL_RADIUS;

  display.fillCircle(cx, cy, 1, SSD1306_WHITE);
  display.fillCircle(cx - offset, cy, r, SSD1306_WHITE);
  display.fillCircle(cx + offset, cy, r, SSD1306_WHITE);
  display.fillCircle(cx, cy - offset, r, SSD1306_WHITE);
  display.fillCircle(cx, cy + offset, r, SSD1306_WHITE);
}

void drawFood(Adafruit_SSD1306 &display, const Food &food) {
  if (!food.active) return;
  display.drawPixel((int)roundf(food.pos.x), (int)roundf(food.pos.y), SSD1306_WHITE);
}

void drawFish(Adafruit_SSD1306 &display, const Fish &fish) {
  float cosA = cosf(fish.heading);
  float sinA = sinf(fish.heading);
  float cx = fish.pos.x;
  float cy = fish.pos.y;

  // Body: three overlapping circles, tapering toward the head (+x local).
  int bx, by;
  rotatePoint(1.2f, 0, cosA, sinA, cx, cy, bx, by);
  display.fillCircle(bx, by, 1, SSD1306_WHITE);
  rotatePoint(0, 0, cosA, sinA, cx, cy, bx, by);
  display.fillCircle(bx, by, (int)FISH_BODY_RADIUS, SSD1306_WHITE);
  rotatePoint(-1.0f, 0, cosA, sinA, cx, cy, bx, by);
  display.fillCircle(bx, by, (int)FISH_BODY_RADIUS - 1, SSD1306_WHITE);

  // Tail: a triangle behind the body, tip swaying side to side over time.
  float wiggle = sinf(fish.wigglePhase) * FISH_WIGGLE_AMPLITUDE;
  int t0x, t0y, t1x, t1y, t2x, t2y;
  rotatePoint(-1.5f, -FISH_TAIL_SPREAD, cosA, sinA, cx, cy, t0x, t0y);
  rotatePoint(-1.5f, FISH_TAIL_SPREAD, cosA, sinA, cx, cy, t1x, t1y);
  rotatePoint(-1.5f - FISH_TAIL_LENGTH, wiggle, cosA, sinA, cx, cy, t2x, t2y);
  display.fillTriangle(t0x, t0y, t1x, t1y, t2x, t2y, SSD1306_WHITE);
}
