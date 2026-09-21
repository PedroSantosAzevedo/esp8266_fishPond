#pragma once
#include <Adafruit_SSD1306.h>
#include "fish.h"
#include "food.h"
#include "leaf.h"
#include "ripple.h"
#include "rain.h"

void drawLeaf(Adafruit_SSD1306 &display, const Leaf &leaf);
void drawFood(Adafruit_SSD1306 &display, const Food &food);
void drawFish(Adafruit_SSD1306 &display, const Fish &fish);
void drawRipple(Adafruit_SSD1306 &display, const Ripple &ripple);
void drawRaindrop(Adafruit_SSD1306 &display, const Raindrop &drop);
