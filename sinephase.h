#ifndef SINEPHASE_H
#define SINEPHASE_H

// This routine modifies phases of a sinewave based on sampleavg

void sinephase() {   //16

// Local definitions

// Persistent local variables

// Temporary local variables


  for (int i=0; i<7; i++) {

    int hue = sin8(i*4+sampleavg) + sin8(i*4+beatsin16(6,-100,100) + sin8(i*4+beatsin16(13,-20,50)) - sin8(i*4+sampleavg));

    int bri = hue;
    bri = bri*bri/255;
    bri = bri*bri/255;        // Remember, it's already halved, so is equivalent to v = v*v*v*v
    //leds[i] = ColorFromPalette(currentPalette, hue, bri, currentBlending);
    for (int j=0; j<12; j++) {
      leds[matrice[i][j]] = ColorFromPalette(currentPalette, hue+10*j, bri, currentBlending);
      //leds[matrice[i][0]] = ColorFromPalette(currentPalette, hue, bri, currentBlending);
      //leds[matrice[i][0]] = ColorFromPalette(currentPalette, hue, bri, currentBlending);
    }
  }

} // sinephase()


#endif
