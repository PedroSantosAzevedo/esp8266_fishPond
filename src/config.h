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
#define NUM_FISH 3
#define NUM_LEAVES 4
#define NUM_FOOD 1

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

#define LEAF_AVOID_RADIUS 12.0f      // px: fish start steering away from leaves
#define LEAF_AVOID_WEIGHT 2.0f
#define LEAF_HARD_RADIUS 6.0f        // px: hard positional correction, last-resort anti-overlap

#define BOUNDARY_MARGIN 6.0f         // px: soft steering margin from screen edges
#define BOUNDARY_WEIGHT 2.5f

#define CHASE_SWITCH_MARGIN 4.0f     // px: hysteresis so the "closest fish" doesn't flap between two fish
#define EAT_DISTANCE 3.0f            // px: how close the chasing fish must get to eat the food

#define LEAF_SHOULD_COLIDE 0          // 1 = leaves are hard obstacles, 0 = leaves are soft obstacles

// ================= Fish body (trail-follow segments) =================
// The body is not a rigid rotated sprite: each fish remembers its last
// TRAIL_LEN positions, and NUM_BODY_SEGMENTS points are sampled out of that
// history (SEGMENT_GAP_FRAMES apart) to become the body's spine. Because the
// spine is literally the path the fish swam, it naturally curves into an
// S-shape through turns, the same look as a chain/IK fish body, without
// having to solve any per-frame angle constraints.
#define NUM_BODY_SEGMENTS 6           // head + 5 trailing body points
#define SEGMENT_GAP_FRAMES 8          // frames of trail history between segments
#define TRAIL_LEN ((NUM_BODY_SEGMENTS - 1) * SEGMENT_GAP_FRAMES + 2)
// Tuned for the FISH_SPEED above (visual body length ~= (NUM_BODY_SEGMENTS-1)
// * SEGMENT_GAP_FRAMES * FISH_SPEED px) - if FISH_SPEED changes a lot,
// SEGMENT_GAP_FRAMES may need adjusting to keep the fish a sensible size.
#define FISH_MAX_HALF_WIDTH 2.6f      // px, half-width at the widest body segment
#define TAIL_FIN_LENGTH 3.5f          // px, how far the tail fin extends past the last segment
#define FISH_WIGGLE_SPEED 0.35f       // radians of tail phase advanced per frame
#define FISH_WIGGLE_AMPLITUDE 1.6f    // px of tail-tip sideways sway

// Pectoral fins: a small triangle on each side, attached to one body
// segment. They get their motion for free from that segment's own
// tangent/perpendicular (already computed to draw the body), so they
// follow the body's curve and heading with no extra trig per frame.
#define FIN_ATTACH_SEGMENT 1          // which body segment the fins attach to
#define FIN_LENGTH 1.5f               // px, how far a fin sticks out past the body edge
#define FIN_SWEEP 1.5f                // px, how far back the fin tip is swept

// ================= Leaf (lily pad) visual size =================
#define LEAF_RADIUS 6.0f              // px
#define LEAF_NOTCH_HALF_ANGLE 0.35f   // radians, half-width of the pac-man-style notch wedge
