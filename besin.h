#ifndef BESIN_H
#define BESIN_H

void besin() {                                           // Add a Perlin noise soundbar. This looks really cool.
  // Shows you don't need a crap ton of code for a cool effect.
  // Local definitions

  // Persistent local variables

  // Temporary local variables
  for (int i = 0; i < 6; i++) {
    leds[matrice[i][0]] = ColorFromPalette(currentPalette, millis(), sampleavg, NOBLEND);
  }


  waveitcol();                                              // Move the pixels to the left/right, but not too fast.

  fadeToBlackBy(leds, NUM_LEDS, 12);                      // Fade the center, while waveit moves everything out to the edges.

} // besin()

#endif
