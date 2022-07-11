#ifndef NOISEPAL_H
#define NOISEPAL_H

void noisepal() {                                                              // Create fire based on noise and sampleavg. 

// Local definitions
  #define xscale 20                                                             // How far apart they are
  #define yscale 3                                                              // How fast they move

// Persistent local variables

// Temporary local variables
  static uint16_t dist;                                                     // A random number for our noise generator.
  
  for(int i = 0; i < 12; i++) {                                       // Just ONE loop to fill up the LED array as all of the pixels change.
    uint8_t index0 = inoise8(i*xscale, dist+i*yscale);                         // Get a value from the noise function. I'm using both x and y axis.
    uint8_t index1 = inoise8(i*xscale+5, dist+i*yscale+20);
    uint8_t index2 = inoise8(i*xscale+10, dist+i*yscale+40);
    uint8_t index3 = inoise8(i*xscale+15, dist+i*yscale+60);                         // Get a value from the noise function. I'm using both x and y axis.
    uint8_t index4 = inoise8(i*xscale+20, dist+i*yscale+80);
    uint8_t index5 = inoise8(i*xscale+25, dist+i*yscale+100);
    uint8_t index6 = inoise8(i*xscale+25, dist+i*yscale+120);
    
    leds[matrice[0][i]] = ColorFromPalette(currentPalette, index0, 255, LINEARBLEND);    // With that value, look up the 8 bit colour palette value and assign it to the current LED.
    leds[matrice[1][i]] = ColorFromPalette(currentPalette, index1, 255, LINEARBLEND);
    leds[matrice[2][i]] = ColorFromPalette(currentPalette, index2, 255, LINEARBLEND);
    leds[matrice[3][i]] = ColorFromPalette(currentPalette, index3, 255, LINEARBLEND);    // With that value, look up the 8 bit colour palette value and assign it to the current LED.
    leds[matrice[4][i]] = ColorFromPalette(currentPalette, index4, 255, LINEARBLEND);
    leds[matrice[5][i]] = ColorFromPalette(currentPalette, index5, 255, LINEARBLEND);
    leds[matrice[6][i]] = ColorFromPalette(currentPalette, index6, 255, LINEARBLEND);
  }
  dist += beatsin8(10,1, 4);                                                // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.
                                                                            // In some sketches, I've used millis() instead of an incremented counter. Works a treat.
} // noisepal()

#endif
