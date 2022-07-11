#ifndef NOISEWIDE_H
#define NOISEWIDE_H

void noisewide() { //7

// Local definitions
  #define GRAVITY 8

// Persistent local variables
  static uint8_t topLED;
  static int gravityCounter = 0;

// Temporary local variables
//  uint8_t tempsamp = constrain(sampleavg/4,0,NUM_LEDS/2-1);                       // Keep the sample from overflowing.
  uint8_t tempsamp = constrain(sampleavg/12,0,12);                       // Keep the sample from overflowing.

  fadeToBlackBy(leds, NUM_LEDS, 160);
    
  for (int i=0; i<tempsamp; i++) {
    uint8_t index = inoise8(i*sampleavg+millis(), 5000+i*sampleavg); 

    //leds[matrice[0][i]] = ColorFromPalette(currentPalette, index, sampleavg-15, currentBlending);       //on réessaie
    leds[matrice[1][i]] = ColorFromPalette(currentPalette, index, sampleavg, currentBlending);
    leds[matrice[2][i]] = ColorFromPalette(currentPalette, index, sampleavg-15, currentBlending);
    leds[matrice[3][i]] = ColorFromPalette(currentPalette, index, sampleavg-15, currentBlending);
    leds[matrice[4][i]] = ColorFromPalette(currentPalette, index, sampleavg, currentBlending);
    leds[matrice[5][i]] = ColorFromPalette(currentPalette, index, sampleavg-15, currentBlending);
    leds[matrice[6][i]] = ColorFromPalette(currentPalette, index, sampleavg-30, currentBlending);

//
//    leds[NUM_LEDS/2-i] = ColorFromPalette(currentPalette, index, sampleavg, currentBlending);
//    leds[NUM_LEDS/2+i] = ColorFromPalette(currentPalette, index, sampleavg, currentBlending);
  }

  if (tempsamp >= topLED)
    topLED = tempsamp;
  else if (gravityCounter % GRAVITY == 0)
    topLED--;

  if (topLED > 0) {
      leds[matrice[1][topLED]] = ColorFromPalette(currentPalette, millis(), 255, LINEARBLEND);                       // LED falls when the volume goes down.
      leds[matrice[4][topLED]] = ColorFromPalette(currentPalette, millis(), 255, LINEARBLEND); // LED falls when the volume goes down.

      //leds[topLED+NUM_LEDS/2] = ColorFromPalette(currentPalette, millis(), 255, LINEARBLEND); // LED falls when the volume goes down.

  }
  
  gravityCounter = (gravityCounter + 1) % GRAVITY;
    
} // noisewide()

#endif
