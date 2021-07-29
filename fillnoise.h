#ifndef FILLNOISE_H
#define FILLNOISE_H

void fillnoise() {                                                             // Another perlin noise based routine.
  // Local definitions
#define xscale 160
#define yscale 160

  // Persistent local variables
  static int16_t xdist;                                                         // A random number for our noise generator.
  static int16_t ydist;

 
  uint8_t samp = map8(sampleavg,1,17);
   //if (sampleavg > 17) sampleavg = 17;                               // Clip the sampleavg to maximize at 19.
  for (int i= (17-samp/2)/2; i<(17+samp/2)/2; i++) {      // The louder the sound, the wider the soundbar.
    uint8_t index = inoise8(i*samp+xdist, ydist+i*samp);              // Get a value from the noise function. I'm using both x and y axis.
    showColumn(i, index, sampleavg);                                             //version fausse 2D 
    //leds[i] = ColorFromPalette(currentPalette, index, sampleavg, LINEARBLEND);  // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }                                                                             // Effect is a NOISE bar the width of sampleavg. Very fun. By Andrew Tuline.
                                              // Effect is a NOISE bar the width of sampleavg. Very fun. By Andrew Tuline.

  xdist = xdist + beatsin8(5, 0, 3);                                            // Moving forward in the NOISE field, but with a sine motion.
  ydist = ydist + beatsin8(4, 0, 3);                                            // Moving sideways in the NOISE field, but with a sine motion.


  waveit();                                                                     // Move the pixels to the left/right, but not too fast.
  //
    fadeToBlackBy(leds+7, 2, 128);                                     // Fade the center, while waveit moves everything out to the edges.
    fadeToBlackBy(leds+23, 2, 128);
    fadeToBlackBy(leds+39, 2, 128);
//  for (int j = 0 ; j < 5; j++) {
//    fadeToBlackBy(leds+j+4, 2, 128);
//  }

} // fillnoisecolonne()

#endif
