#ifndef RONDS_H
#define RONDS_H

// dessine un cercle sur samplepeak

void ronds(){

   static uint8_t ixe = 0;
   static uint8_t igrec = 0;

  uint8_t thisbright = 180;
  uint8_t colorIndex = 128;
   
  if (samplepeak) {                                                               // Trigger a rainbow with a peak.
    
    samplepeak = 0;                                                               // Got a peak, now reset it.
    
    ixe = random8(2);
    igrec = random8(16);
    
    leds[matrice[ixe][igrec]] = ColorFromPalette( currentPalette, colorIndex, thisbright, currentBlending);
    leds[matrice[ixe+1][igrec+1]] = ColorFromPalette( currentPalette, colorIndex+30, thisbright-30, currentBlending);
    leds[matrice[ixe-1][igrec-1]] = ColorFromPalette( currentPalette, colorIndex+30, thisbright-30, currentBlending);
    leds[matrice[ixe+2][igrec+2]] = ColorFromPalette( currentPalette, colorIndex+60, thisbright-60, currentBlending);
    leds[matrice[ixe-2][igrec-2]] = ColorFromPalette( currentPalette, colorIndex+60, thisbright-60, currentBlending);

    //for i++ leds(x+i,y+1)(bright-20i, index)
    //ou leds (x+i, y+i) = leds (x,y)
  }
  
  EVERY_N_MILLIS(150){fadeToBlackBy(leds, NUM_LEDS, 200);}                                              // Fade saccadé.

} //ronds

#endif
