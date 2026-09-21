#include "controls.h"
#include "config.h"
#include <Arduino.h>

void initControls(Controls &c) {
  c.rainEnabled = RAIN_DEFAULT_ON;
  c.ambientRipplesEnabled = AMBIENT_RIPPLES_DEFAULT_ON;
  c.wind = 0;
}

static void printStatus(const Controls &c) {
  Serial.printf("rain=%s ripples=%s wind=%d\n", c.rainEnabled ? "on" : "off",
                c.ambientRipplesEnabled ? "on" : "off", c.wind);
}

void handleSerialControls(Controls &c) {
  while (Serial.available()) {
    char ch = Serial.read();
    switch (ch) {
      case 'r': c.rainEnabled = !c.rainEnabled; break;
      case 'p': c.ambientRipplesEnabled = !c.ambientRipplesEnabled; break;
      case 'a': if (c.wind > -RAIN_MAX_WIND) c.wind--; break;
      case 'd': if (c.wind < RAIN_MAX_WIND) c.wind++; break;
      case 's': c.wind = 0; break;
      case '?':
        Serial.println("r=rain on/off  p=ripples on/off  a=wind left  d=wind right  s=no wind");
        break;
      default: continue;
    }
    printStatus(c);
  }
}
