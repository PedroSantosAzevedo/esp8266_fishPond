#pragma once

// Runtime switches, changed through serial commands (115200 baud).
struct Controls {
  bool rainEnabled;
  bool ambientRipplesEnabled;
  int wind;   // -RAIN_MAX_WIND..+RAIN_MAX_WIND, negative = drifts left
};

void initControls(Controls &c);

// Reads pending serial characters and applies them:
//   r  rain on/off      p  ambient ripples on/off
//   a  wind left        d  wind right        s  no wind (straight down)
//   ?  print help
void handleSerialControls(Controls &c);
