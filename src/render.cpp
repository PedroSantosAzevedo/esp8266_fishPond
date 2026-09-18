#include "render.h"
#include "config.h"
#include <math.h>

// Fish body rendering, inspired by chain/IK fish-body sketches (a spine of
// segments with a tapered width profile, drawn as a smooth silhouette) but
// adapted to be cheap on an ESP8266: instead of solving per-frame angle
// constraints along a segment chain, the spine points are just sampled from
// each fish's recorded trail (see fish.h/config.h). That means no trig is
// needed to place the segments at all - only a normalize per segment to get
// the perpendicular (width) direction, plus one sin() for the tail wiggle.
static const float BODY_WIDTH_PROFILE[NUM_BODY_SEGMENTS] = {0.45f, 0.85f, 1.0f, 0.85f, 0.6f, 0.32f};

void drawLeaf(Adafruit_SSD1306 &display, const Leaf &leaf) {
  int cx = (int)roundf(leaf.pos.x);
  int cy = (int)roundf(leaf.pos.y);
  display.fillCircle(cx, cy, (int)LEAF_RADIUS, SSD1306_WHITE);
  // Pac-Man-style notch bitten out of the lily pad, precomputed at init.
  display.fillTriangle(cx, cy, cx + (int)roundf(leaf.notchA.x), cy + (int)roundf(leaf.notchA.y),
                        cx + (int)roundf(leaf.notchB.x), cy + (int)roundf(leaf.notchB.y),
                        SSD1306_BLACK);
}

void drawFood(Adafruit_SSD1306 &display, const Food &food) {
  if (!food.active) return;
  display.drawPixel((int)roundf(food.pos.x), (int)roundf(food.pos.y), SSD1306_WHITE);
}

void drawFish(Adafruit_SSD1306 &display, const Fish &fish) {
  Vec2 seg[NUM_BODY_SEGMENTS];
  Vec2 tangent[NUM_BODY_SEGMENTS];
  Vec2 perp[NUM_BODY_SEGMENTS];
  int Lx[NUM_BODY_SEGMENTS], Ly[NUM_BODY_SEGMENTS], Rx[NUM_BODY_SEGMENTS], Ry[NUM_BODY_SEGMENTS];

  for (int i = 0; i < NUM_BODY_SEGMENTS; i++) {
    seg[i] = fish.trail[i * SEGMENT_GAP_FRAMES];
  }

  for (int i = 0; i < NUM_BODY_SEGMENTS; i++) {
    Vec2 t;
    if (i == 0) {
      t = seg[0] - seg[1];
    } else if (i == NUM_BODY_SEGMENTS - 1) {
      t = seg[i - 1] - seg[i];
    } else {
      t = seg[i - 1] - seg[i + 1];
    }
    t = t.normalized();
    tangent[i] = t;
    perp[i] = Vec2(-t.y, t.x);

    float halfW = BODY_WIDTH_PROFILE[i] * FISH_MAX_HALF_WIDTH;
    Vec2 l = seg[i] + perp[i] * halfW;
    Vec2 r = seg[i] - perp[i] * halfW;
    Lx[i] = (int)roundf(l.x);
    Ly[i] = (int)roundf(l.y);
    Rx[i] = (int)roundf(r.x);
    Ry[i] = (int)roundf(r.y);
  }

  // Rounded nose.
  display.fillCircle((int)roundf(seg[0].x), (int)roundf(seg[0].y),
                      (int)roundf(BODY_WIDTH_PROFILE[0] * FISH_MAX_HALF_WIDTH), SSD1306_WHITE);

  // Pectoral fins: one small triangle per side, attached to FIN_ATTACH_SEGMENT.
  // The base runs from the spine point to the body edge (a real wedge, not
  // two nearly-coincident edge points a segment apart), with the tip swept
  // out and back from the spine. Drawn before the body strip so the body
  // cleanly covers the base, leaving only the part past the edge visible.
  {
    int a = FIN_ATTACH_SEGMENT;
    int segX = (int)roundf(seg[a].x), segY = (int)roundf(seg[a].y);
    float halfW = BODY_WIDTH_PROFILE[a] * FISH_MAX_HALF_WIDTH;

    Vec2 finTipL = seg[a] + perp[a] * (halfW + FIN_LENGTH) - tangent[a] * FIN_SWEEP;
    display.fillTriangle(segX, segY, Lx[a], Ly[a], (int)roundf(finTipL.x), (int)roundf(finTipL.y),
                          SSD1306_WHITE);

    Vec2 finTipR = seg[a] - perp[a] * (halfW + FIN_LENGTH) - tangent[a] * FIN_SWEEP;
    display.fillTriangle(segX, segY, Rx[a], Ry[a], (int)roundf(finTipR.x), (int)roundf(finTipR.y),
                          SSD1306_WHITE);
  }

  // Tapered body as a triangle strip between consecutive left/right edges.
  for (int i = 0; i < NUM_BODY_SEGMENTS - 1; i++) {
    display.fillTriangle(Lx[i], Ly[i], Rx[i], Ry[i], Lx[i + 1], Ly[i + 1], SSD1306_WHITE);
    display.fillTriangle(Rx[i], Ry[i], Lx[i + 1], Ly[i + 1], Rx[i + 1], Ry[i + 1], SSD1306_WHITE);
  }

  // Tail fin: swishes side to side using the tail segment's own tangent/perpendicular.
  int tailIdx = NUM_BODY_SEGMENTS - 1;
  Vec2 tailDir = (seg[tailIdx - 1] - seg[tailIdx]).normalized();
  float wiggle = sinf(fish.wigglePhase) * FISH_WIGGLE_AMPLITUDE;
  Vec2 tailTip = seg[tailIdx] - tailDir * TAIL_FIN_LENGTH + perp[tailIdx] * wiggle;
  display.fillTriangle(Lx[tailIdx], Ly[tailIdx], Rx[tailIdx], Ry[tailIdx],
                        (int)roundf(tailTip.x), (int)roundf(tailTip.y), SSD1306_WHITE);
}
