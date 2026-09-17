#pragma once

// ================= Display =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C   // most 0.96" SSD1306 boards use 0x3C; some use 0x3D

// ================= I2C pins =================
// Raw GPIO numbers so this works on any ESP8266 board variant.
// GPIO4 = "D2" and GPIO5 = "D1" on Wemos D1 Mini / NodeMCU silkscreens.
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

// ================= Scene population =================
#define NUM_FISH 5
#define NUM_FLOWERS 2
#define NUM_FOOD 3

// ================= Timing =================
#define FRAME_INTERVAL_MS 33UL       // ~30 fps target
#define FOOD_MIN_RESPAWN_MS 1500UL
#define FOOD_MAX_RESPAWN_MS 5000UL

// ================= Fish motion tuning =================
#define FISH_SPEED 0.3f             // px moved per frame
#define FISH_MAX_TURN_RATE 0.12f     // max heading change (radians) per frame
#define WANDER_JITTER 0.20f          // random heading nudge (radians) per frame while wandering
#define WANDER_WEIGHT 1.0f
#define CHASE_WEIGHT 1.6f

#define SEPARATION_DIST 5.0f        // px: fish start steering away from each other
#define SEPARATION_WEIGHT 2.2f
#define SEPARATION_HARD_DIST 5.0f    // px: hard positional correction, last-resort anti-overlap

#define FLOWER_AVOID_RADIUS 12.0f    // px: fish start steering away from flowers
#define FLOWER_AVOID_WEIGHT 2.0f
#define FLOWER_HARD_RADIUS 6.0f      // px: hard positional correction, last-resort anti-overlap

#define BOUNDARY_MARGIN 6.0f         // px: soft steering margin from screen edges
#define BOUNDARY_WEIGHT 2.5f

#define CHASE_SWITCH_MARGIN 4.0f     // px: hysteresis so the "closest fish" doesn't flap between two fish
#define EAT_DISTANCE 3.0f            // px: how close the chasing fish must get to eat the food

// ================= Fish visual size =================
#define FISH_BODY_RADIUS 2.0f
#define FISH_TAIL_LENGTH 3.0f
#define FISH_TAIL_SPREAD 1.3f
#define FISH_WIGGLE_SPEED 0.35f      // radians of tail phase advanced per frame
#define FISH_WIGGLE_AMPLITUDE 1.4f   // px of tail-tip sideways sway

// ================= Flower visual size =================
#define FLOWER_PETAL_OFFSET 2.0f
#define FLOWER_PETAL_RADIUS 1.0f
