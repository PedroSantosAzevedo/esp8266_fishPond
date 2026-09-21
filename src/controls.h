#pragma once

enum RainMode { RAIN_ON, RAIN_OFF_RIPPLES, RAIN_OFF_NO_RIPPLES };
enum WindDir { WIND_OFF, WIND_LEFT, WIND_RIGHT };

// Runtime switches, changed with three buttons (see config.h for pins).
struct Controls {
  // What the buttons change:
  RainMode rainMode;
  WindDir windDir;
  int windIntensity;      // 1..RAIN_MAX_WIND

  // Derived from the above; this is what the simulation reads.
  bool rainEnabled;
  bool ambientRipplesEnabled;
  int wind;               // px/frame level, negative = drifts left, 0 = straight down
};

void initControls(Controls &c);

// Polls the buttons (debounced) and the serial port, then updates c.
// Serial mirrors the buttons for debugging (115200 baud): r = rain mode,
// w = wind direction, i = wind intensity, ? = help.
void updateControls(Controls &c);
