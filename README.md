# ESP8266 Koi Pond Simulation

A small koi-pond animation for an ESP8266 driving a 128x64 SSD1306 OLED over I2C.
Koi fish wander a lake dotted with flowers, chase down single-pixel food that
spawns at random locations, and avoid crashing into each other or into the
flowers. Only the fish closest to the active food chases it — the others keep
wandering.

## Features

- 3 koi fish rendered as small rotated silhouettes (tapered body + wiggling tail)
- 4 static flowers acting as obstacles
- Single-pixel food that spawns at random, non-overlapping-with-flowers positions
- "Closest fish only" chase logic with hysteresis (no flapping between two
  near-equidistant fish)
- Steering-based movement: wander / chase / separation (anti-fish-crash) /
  flower avoidance / boundary avoidance, each as a weighted force, plus a
  hard positional correction as a last-resort safety net so fish never
  visually overlap a flower or another fish
- All tunables (speeds, counts, radii, timing) centralized in one config file

## Hardware Required

| Qty | Part | Notes |
|-----|------|-------|
| 1 | ESP8266 dev board | Wemos D1 Mini or NodeMCU recommended (this project targets `d1_mini` by default) |
| 1 | SSD1306 OLED display, 128x64, I2C | Common 0.96" module, 4-pin (VCC/GND/SCL/SDA); I2C pull-up resistors are normally already on the breakout board |
| 4 | Jumper wires | Female-to-female if both boards have pin headers |
| 1 | Micro USB cable | Power + programming |

No breadboard is strictly required — 4 direct jumper wires are enough.

## Wiring

![Wiring diagram](docs/wiring_diagram.svg)

| ESP8266 pin | OLED pin | Purpose |
|---|---|---|
| 3V3 | VCC | Power |
| GND | GND | Ground |
| D1 (GPIO5) | SCL | I2C clock |
| D2 (GPIO4) | SDA | I2C data |

These are the default I2C pins on ESP8266 boards and match `I2C_SDA_PIN` /
`I2C_SCL_PIN` in [`src/config.h`](src/config.h) (set as raw GPIO numbers so
the code works on any ESP8266 board variant, not just boards with `D1`/`D2`
silkscreen labels). If your display doesn't light up, double check the I2C
address too — most SSD1306 modules are `0x3C`, but some are `0x3D` (see
Troubleshooting below).

## Software Setup

This is a [PlatformIO](https://platformio.org/) project.

1. Install PlatformIO (either the standalone CLI, or the VS Code extension).
2. Open this folder as a PlatformIO project.
3. Connect the ESP8266 via USB.
4. Build and upload:

```bash
pio run --target upload
```

5. Optional: watch serial output (not currently used for logging, but handy
   for debugging):

```bash
pio device monitor
```

Libraries (`Adafruit SSD1306`, `Adafruit GFX Library`, `Adafruit BusIO`) are
declared in `platformio.ini` and are fetched automatically on first build.

## Project Structure

```
platformio.ini          PlatformIO build configuration
src/
  main.cpp               setup()/loop(), scene init, frame timing
  config.h                all tunable constants (display, timing, motion, visuals)
  vec2.h                  minimal 2D vector math + random-float helper
  fish.h / fish.cpp       Fish struct, steering/movement, chaser assignment
  food.h / food.cpp       Food struct, eat detection, respawn logic
  flower.h                Flower struct (static obstacles)
  render.h / render.cpp   drawing functions for fish/flowers/food
docs/
  wiring_diagram.svg      wiring diagram embedded above
```

## How It Works

**Movement** is a simplified steering-behavior model. Each frame, every fish
computes a handful of force vectors and sums them:

- **Wander** (when not chasing food): a slow random walk in heading.
- **Chase** (when assigned as the closest fish to active food): steer
  directly toward the food.
- **Separation**: steer away from any other fish closer than
  `SEPARATION_DIST`.
- **Flower avoidance**: steer away from any flower closer than
  `FLOWER_AVOID_RADIUS`.
- **Boundary avoidance**: steer away from the screen edges.

The summed force sets a target heading; the fish's actual heading turns
toward it at a capped rate (`FISH_MAX_TURN_RATE`) so motion stays smooth.
After moving, a hard positional correction pushes the fish out of any flower
or fish it's still overlapping — this is the actual guarantee against visible
crashes, with the soft steering forces above just making it look natural
rather than jarring.

**Closest-fish-chases logic**: every frame, `assignChaser()` finds the fish
nearest to the active food and puts it in `CHASE` state, forcing every other
fish back to `WANDER`. A small hysteresis margin (`CHASE_SWITCH_MARGIN`)
prevents the chase from flickering between two fish that are nearly
equidistant.

**Food lifecycle**: one food pixel is active at a time. It spawns at a random
point at least `FLOWER_AVOID_RADIUS` away from every flower. When the
chasing fish gets within `EAT_DISTANCE`, the food is eaten, and a new one
spawns after a random delay between `FOOD_MIN_RESPAWN_MS` and
`FOOD_MAX_RESPAWN_MS`.

**Fish visuals**: rather than a fixed sprite sheet of pre-drawn frames, each
fish is drawn procedurally every frame — three overlapping circles (tapering
toward the head) for the body, and a triangle for the tail whose tip sways
side to side over time. Both are rotated to the fish's exact current heading
using simple 2D rotation, which gives smooth rotation at any angle (not just
8 snapped directions) and a continuous swimming animation, for effectively
no extra flash usage.

## Configuration / Tuning

All tunable values live in [`src/config.h`](src/config.h):

- **Scene population**: `NUM_FISH`, `NUM_FLOWERS`, `NUM_FOOD`
- **Timing**: `FRAME_INTERVAL_MS`, `FOOD_MIN_RESPAWN_MS`, `FOOD_MAX_RESPAWN_MS`
- **Motion**: `FISH_SPEED`, `FISH_MAX_TURN_RATE`, `WANDER_JITTER`, and the
  weight/distance constants for separation, flower avoidance and boundary
  avoidance
- **Visuals**: fish body/tail size and wiggle, flower petal size

Increasing `NUM_FISH` or `NUM_FLOWERS` beyond a handful on a 128x64 screen
will make the avoidance behaviors work much harder and may need looser
`SEPARATION_DIST` / `FLOWER_AVOID_RADIUS` values to keep things from
looking crowded.

## Troubleshooting

- **Blank screen / boot loops on `display.begin()` failure**: the code halts
  in a loop if the SSD1306 isn't found at `SCREEN_ADDRESS`. Try `0x3D`
  instead of `0x3C` in `config.h`, and double check wiring/power.
- **Fish look jittery**: lower `FISH_MAX_TURN_RATE` or `WANDER_JITTER` in
  `config.h`.
- **Fish seem to ignore food**: check `EAT_DISTANCE` isn't too small relative
  to `FISH_SPEED` (a fast fish can overshoot a very small eat radius between
  frames).

## Possible Extensions

- Multiple simultaneous food pixels (`NUM_FOOD > 1`, `assignChaser` already
  loops over foods generically)
- Bubble particles or a day/night background dim cycle
- Button/PIR input to summon food at a chosen spot
- Persist a "fish fed" counter to EEPROM/LittleFS
