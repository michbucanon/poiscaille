#ifndef MATRIX_H
#define MATRIX_H

void matrix() {                                                  // A 'Matrix' like display using sampleavg for brightness.

// Local definitions

// Persistent local variables

// Temporary local variables
static uint8_t thishue = 0;

  if (thisdir == 1) {
        showLed(1,0, thishue++, sampleavg);
        showLed(1,1, 12+thishue++, sampleavg);
        showLed(1,2, 24+thishue++, sampleavg);
  } else {
      leds[NUM_LEDS-1] = ColorFromPalette( currentPalette, millis(), sample, currentBlending);
  }

  if (thisdir == 1) {
     for (int i = 8; i > 0; i--) {
    leds[matrice[0][i]] = leds[matrice[0][i-1]];
    leds[matrice[1][i]] = leds[matrice[1][i-1]];
    leds[matrice[2][i]] = leds[matrice[2][i-1]];
    leds[matrice[3][i]] = leds[matrice[0][i-1]];
    leds[matrice[4][i]] = leds[matrice[1][i-1]];
    leds[matrice[5][i]] = leds[matrice[2][i-1]];
  } }else {
    for (int i = 0; i < NUM_LEDS-1 ; i++ ) leds[i] = leds[i+1];
  }

} // matrix()

#endif
