#pragma once
#include <Arduino.h>
#include <math.h>

struct Vec2 {
  float x = 0, y = 0;

  Vec2() {}
  Vec2(float _x, float _y) : x(_x), y(_y) {}

  Vec2 operator+(const Vec2 &o) const { return Vec2(x + o.x, y + o.y); }
  Vec2 operator-(const Vec2 &o) const { return Vec2(x - o.x, y - o.y); }
  Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
  Vec2 &operator+=(const Vec2 &o) {
    x += o.x;
    y += o.y;
    return *this;
  }

  float length() const { return sqrtf(x * x + y * y); }

  Vec2 normalized() const {
    float len = length();
    if (len < 0.0001f) return Vec2(0, 0);
    return Vec2(x / len, y / len);
  }
};

// Uniform random float in [lo, hi].
inline float randomFloat(float lo, float hi) {
  return lo + (hi - lo) * (float)random(0, 10001) / 10000.0f;
}
