#ifndef RIPPLE_H
#define RIPPLE_H

// Create a ripple when a peak is detected. Create a new one when a new peak is detected and let the old one fade away.
// This routine can only handle one ripple at a time.

void ripple() {                                                                 // Display ripples triggered by peaks.

// Local definitions
  #define maxsteps 16                                                           // Maximum number of steps.

// Persistent local variables
  static uint8_t colour;                                                        // Ripple colour is based on samples.
  static uint16_t centerx = 0;                                                   // Center of current ripple.
  static uint16_t centery = 0;
  static int8_t step = -1;                                                      // Phase of the ripple as it spreads out.

// Temporary local variables


  if (samplepeak) {samplepeak = 0; step = -1;}                                  // Trigger a new ripple if we have a peak.
  
  fadeToBlackBy(leds, NUM_LEDS, 75);                                            // Fade the strand, where 1 = slow, 255 = fast

  switch (step) {

    case -1:                                                                    // Initialize ripple variables. By Andrew Tuline.
      centerx = random(12);
      centery = random(7);
      colour = (sample) % 255;                                                  // More peaks/s = higher the hue colour.
      step = 0;
      break;

    case 0:
      leds[matrice[centery][centerx]] += ColorFromPalette(currentPalette, colour+millis(), 255, currentBlending); // Display the first pixel of the ripple.
      step ++;
      break;

    case maxsteps:                                                              // At the end of the ripples.
      // step = -1;
      break;

    default:                                                                    // Middle of the ripples.

      //leds[matrice[(centerx + step + 7) % 7][centery]] += ColorFromPalette(currentPalette, colour+millis(), 255/step*2, currentBlending);  // A spreading and fading pattern up the strand.
      //leds[matrice[(centerx - step + 7) % 7][centery]] += ColorFromPalette(currentPalette, colour+millis(), 255/step*2, currentBlending);  // A spreading and fading pattern down the strand.
      leds[matrice[centery][(centerx + step + 5) % 12]] += ColorFromPalette(currentPalette, colour+millis(), 255/step*2, currentBlending);  // A spreading and fading pattern up the strand.
      leds[matrice[centery][(centerx - step + 5) % 12]] += ColorFromPalette(currentPalette, colour+millis(), 255/step*2, currentBlending);  // A spreading and fading pattern down the strand.
      leds[matrice[(centery+1)%7][(centerx + step + 5) % 7]] += ColorFromPalette(currentPalette, colour+millis()-75, 64/step*2, currentBlending);  // A spreading and fading pattern up the strand.
      leds[matrice[(centery+1)%7][(centerx - step - 5) % 7]] += ColorFromPalette(currentPalette, colour+millis()-75, 64/step*2, currentBlending);  // A spreading and fading pattern down the strand.
      leds[matrice[(centery-1)%7][(centerx + step + 5) % 7]] += ColorFromPalette(currentPalette, colour+millis()+75, 64/step*2, currentBlending);  // A spreading and fading pattern up the strand.
      leds[matrice[(centery-1)%7][(centerx - step - 5) % 7]] += ColorFromPalette(currentPalette, colour+millis()+75, 64/step*2, currentBlending);  // A spreading and fading pattern down the strand.
      step ++;                                                                  // Next step.
      break;  
      
  } // switch step
  
} // ripple()

#endif
