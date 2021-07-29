#ifndef PLASMA_H
#define PLASMA_H

void plasma() {

// Local definitions

// Persistent local variables
  static int16_t thisphase = 0;                                                 // Phase of a cubicwave8.
  static int16_t thatphase = 0;                                                 // Phase of the cos8.

// Temporary local variables
  uint16_t thisbright;
  uint16_t colorIndex;


  thisphase += beatsin8(6,-4,4);                                                // You can change direction and speed individually.
  thatphase += beatsin8(7,-4,4);                                                // Two phase values to make a complex pattern. By Andrew Tuline.

  for (int k=0; k<8; k++) {                                              // For each of the LED's in the ligne, set a brightness based on a wave as follows.
    thisbright = cubicwave8((k*8)+thisphase)/2;    
    thisbright += cos8((k*10)+thatphase)/2;                                     // Let's munge the brightness a bit and animate it all with the phases.
    colorIndex=thisbright;
    thisbright = qsuba(thisbright, 280-sampleavg*2.5);                              // qsuba chops off values below a threshold defined by sampleavg. Gives a cool effect. À tweaker, tout reste allumé par fort volume/tweaké, à tester

         for (int j=0; j<6; j++){
          leds[matrice[j][k]] = ColorFromPalette( currentPalette, colorIndex+15*j, thisbright, currentBlending);
         }
        }
    
  }
   
 // plasma()

#endif
