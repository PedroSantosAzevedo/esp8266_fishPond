#pragma once
#include <Adafruit_SSD1306.h>
#include "fish.h"
#include "food.h"
#include "flower.h"

void drawFlower(Adafruit_SSD1306 &display, const Flower &flower);
void drawFood(Adafruit_SSD1306 &display, const Food &food);
void drawFish(Adafruit_SSD1306 &display, const Fish &fish);
