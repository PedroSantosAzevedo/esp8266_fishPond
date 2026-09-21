#include "controls.h"
#include "config.h"
#include <Arduino.h>

struct Button {
  uint8_t pin;
  bool lastReading;          // raw level last time we looked (HIGH = released)
  bool stable;               // debounced level
  unsigned long changedAt;   // when the raw level last changed
};

static Button btnRain = {BTN_RAIN_PIN, HIGH, HIGH, 0};
static Button btnWindDir = {BTN_WIND_DIR_PIN, HIGH, HIGH, 0};
static Button btnWindIntensity = {BTN_WIND_INTENSITY_PIN, HIGH, HIGH, 0};

// Returns true once per press (on the debounced HIGH -> LOW edge).
static bool wasPressed(Button &b) {
  bool reading = digitalRead(b.pin);
  unsigned long now = millis();
  if (reading != b.lastReading) {
    b.lastReading = reading;
    b.changedAt = now;
  }
  if (now - b.changedAt >= BUTTON_DEBOUNCE_MS && reading != b.stable) {
    b.stable = reading;
    return reading == LOW;
  }
  return false;
}

static void refresh(Controls &c) {
  c.rainEnabled = c.rainMode == RAIN_ON;
  // While it rains the rain makes the ripples; ambient ones only in the "no rain" mode that asks for them.
  c.ambientRipplesEnabled = c.rainMode == RAIN_OFF_RIPPLES;
  c.wind = c.windDir == WIND_LEFT ? -c.windIntensity : c.windDir == WIND_RIGHT ? c.windIntensity : 0;
}

static void printStatus(const Controls &c) {
  const char *modes[] = {"rain", "no rain + ripples", "no rain, no ripples"};
  const char *dirs[] = {"off", "left", "right"};
  Serial.printf("mode=%s wind=%s intensity=%d\n", modes[c.rainMode], dirs[c.windDir], c.windIntensity);
}

static void cycleRain(Controls &c) { c.rainMode = (RainMode)((c.rainMode + 1) % 3); }
static void cycleWindDir(Controls &c) { c.windDir = (WindDir)((c.windDir + 1) % 3); }
static void cycleIntensity(Controls &c) { c.windIntensity = c.windIntensity % RAIN_MAX_WIND + 1; }

void initControls(Controls &c) {
  pinMode(BTN_RAIN_PIN, INPUT_PULLUP);
  pinMode(BTN_WIND_DIR_PIN, INPUT_PULLUP);
  pinMode(BTN_WIND_INTENSITY_PIN, INPUT_PULLUP);
  c.rainMode = (RainMode)RAIN_DEFAULT_MODE;
  c.windDir = WIND_OFF;
  c.windIntensity = 1;
  refresh(c);
}

void updateControls(Controls &c) {
  bool changed = false;
  if (wasPressed(btnRain)) { cycleRain(c); changed = true; }
  if (wasPressed(btnWindDir)) { cycleWindDir(c); changed = true; }
  if (wasPressed(btnWindIntensity)) { cycleIntensity(c); changed = true; }

  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == 'r') { cycleRain(c); changed = true; }
    else if (ch == 'w') { cycleWindDir(c); changed = true; }
    else if (ch == 'i') { cycleIntensity(c); changed = true; }
    else if (ch == '?') Serial.println("r=rain mode  w=wind direction  i=wind intensity");
  }

  if (changed) {
    refresh(c);
    printStatus(c);
  }
}
