#ifndef FILLNOISE_H
#define FILLNOISE_H

void fillnoise() {                       //2                                      // Another perlin noise based routine.
  // Local definitions
#define xscale 160
#define yscale 160

  // Persistent local variables
  static int16_t xdist;                                                         // A random number for our noise generator.
  static int16_t ydist;
  uint8_t milieu = 3;

  uint8_t samp = map8(sampleavg, 1, 4);
  //if (sampleavg > 17) sampleavg = 17;                               // Clip the sampleavg to maximize at 19.
  for (int i = (milieu - samp / 2) / 2; i < (milieu + samp / 2) / 2; i++) { // The louder the sound, the wider the soundbar.
    uint8_t index = inoise8(i * samp + xdist, ydist + i * samp);      // Get a value from the noise function. I'm using both x and y axis.
    for (int j = 0; j < 7; j++) {
      leds[matrice[j][i]] = ColorFromPalette(currentPalette, index + 15 * j, sampleavg, LINEARBLEND); // With that value, look up the 8 bit colour palette value and assign it to the current LED.

    }
  }                                                          // Effect is a NOISE bar the width of sampleavg. Very fun. By Andrew Tuline.
  // Effect is a NOISE bar the width of sampleavg. Very fun. By Andrew Tuline.

  xdist = xdist + beatsin8(5, 0, 3);                                            // Moving forward in the NOISE field, but with a sine motion.
  ydist = ydist + beatsin8(4, 0, 3);                                            // Moving sideways in the NOISE field, but with a sine motion.

  for (int i = 8; i > 0; i--) {                           // Move to the right la première ligne.
    leds[i] = leds[i - 1];
  }
  for (int i = 27; i > 17; i--) {                           // Move to the right la deuxième.
    leds[i] = leds[i - 1];
  }
  for (int i = 49; i > 37; i--) {                          // Move to the right la troisième.
    leds[i] = leds[i - 1];
  }


  for (int i = 8; i < 17; i++) {                                      // Move to the left1.
    leds[i] = leds[i + 1];
  }
  for (int i = 27; i < 37; i++) {                                     // Move to the left2.
    leds[i] = leds[i + 1];
  }
  for (int i = 50; i < 61; i++) {                                      // Move to the left3.
    leds[i] = leds[i + 1];
  }                                                                    // Move the pixels to the left/right, but not too fast.
  
  fadeToBlackBy(leds, NUM_LEDS, 32);

} // fillnoisecolonne()

#endif
