#ifndef FIRE_H
#define FIRE_H

void fire() {                                //15                                    // tentons de calibrer notre bidule.
// Local definitions


// Persistent local variables

// Temporary local variables
  uint8_t samp = map8(sampleavg,0,NUM_LEDS);                                                            // Current colour lookup value.
  uint8_t maxv = map8(maxvol,0,NUM_LEDS);
  uint8_t squel = map8(squelch,0,NUM_LEDS);
  
  uint16_t index = 0;
  
  for(int i = 0; i < samp; i++) {                                               //

    leds[i] = ColorFromPalette(currentPalette, index, sampleavg, NOBLEND);      // samplavg allume des leds 
  }                                                                             // 


    
    fadeToBlackBy(leds, NUM_LEDS, 128);               //Fondu au noir

    leds[squel] = CRGB::Red;                        //la valeur du squelch
    leds[maxv] = CRGB::Green;                          // Et celle du seuil qui définit le pic.
    leds[scalvol] = CRGB::Blue                      //échelle

//    if (sampleavg < maxvol) samplepeak = 0;   

    if (samplepeak) {
      
      samplepeak = 0;
      
      leds[46] = CRGB::Yellow;
    }
    
                                                                                                     
} // fire()

#endif
