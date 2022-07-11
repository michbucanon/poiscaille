#ifndef BESIN_H
#define BESIN_H

void besin() {                 //13                          // Add a Perlin noise soundbar. This looks really cool.
  // Shows you don't need a crap ton of code for a cool effect.
  // Local definitions

  // Persistent local variables

  // Temporary local variables
  
  leds[NUM_LEDS/2] = ColorFromPalette(currentPalette, millis(), sampleavg, NOBLEND);
  leds[NUM_LEDS/2-1] = ColorFromPalette(currentPalette, millis(), sampleavg, NOBLEND);
  
 for (int i = NUM_LEDS - 1; i > NUM_LEDS/2; i--) {                             // Move to the right.
    leds[i] = leds[i - 1];
  }

  for (int i = 0; i < NUM_LEDS/2; i++) {                                        // Move to the left.
    leds[i] = leds[i + 1];
  }
  
  fadeToBlackBy(leds, NUM_LEDS, 12);                      // Fade the center, while waveit moves everything out to the edges.

} // besin()

#endif
