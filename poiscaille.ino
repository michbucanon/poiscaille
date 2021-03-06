/*
 *  FastLED Support Forums: https://www.reddit.com/r/FastLED/
 *  

 * 
 * Compile Time Configuration ******************************************************************************************************************************
 * 
 * Set the demorun startup variable to 1 for demo mode or 0 for regular use. It's configured to 0 (non-demo mode) by default.
 * You may need to adjust soundmems.h for your microphone. It's currently configured for an ADMP401.
 * You may need to review the analogreference() in setup() for your microphone as well. It's currently configured for the 3.3V ADMP401.
 *
 * 
 * Notasound Initial Configuration ************************************************************************************************************************
 * 
 * 1) Set your strand length.
 * 
 * You'll need to 'activate' your strand for EEPROM programming. To do so:
 * 
 * - Press B1 to put the Arduino into 'Select strand' mode.
 * - Press the button equivalent to your STRANDID, i.e. C1 to 'activate' your Arduino for EEPROM programming.
 * - Press B2 to decrease the strand length.
 * - Press B3 to increase the strand length.
 * 
 * LED's will light up as 'white' to indicate the strand length. The strand length will be saved to EEPROM after each keypress.
 * Once done, press B1 again or press A3 to reset the Arduino.
 *  
 * 
 * 
 * 
 * 
 * IR Keys and Operation ********************************************************************************************************************************
 * 
 * This configuration uses a 24 button IR remote like the ones provided with 5V LED strands from China. If you use a different one, you'll need to map
 * the codes to the modes in the irtest(); routine in the commands.h file.
 * 
 * The chart below summarizes the commands. Button location uses characters as the row, and numbers are the columns, so C2 is the 3rd row, 2nd column.
 * 
 * 
 * Command                          IR Button location & description
 * --------                         --------------------------------
 * Increase brightness              A1  Increase brightness
 * Decrease brightness              A2  Decrease brightness
 * Reset and set mode 0             A3  Reboots the Arduino in order to sync millis() if using notasound. Factory reset if < 2 seconds.
 * Enable demo mode                 A4  Demo mode cycles through the routines based on the millis() counter. Not a toggle.
 * 
 * Select Arduino                   B1  This allows the EEPROM to be updated. Then press A1 through F4 as configured with STRANDID at compile time. (not A3 or B1 though).
 * Decrease strand length           B2  The # of LED's programmed are white, only if strand is active (via B1 & STRANDID). This is saved in EEPROM. Press B1 again or A3 to reboot when done.
 * Increase strand length           B3  The # of LED's programmed are white, only if strand is active (via B1 & STRANDID). This is saved in EEPROM. Press B1 again or A3 to reboot when done.
 * Palette rotation                 B4  Start palette rotation.
 * 
 * Save palette                     C1  Stop palette rotation and save current palette to EEPROM.
 * Slower animation                 C2  Increase value of EVERY_N_MILLISECONDS() for the current (fixed) display mode.
 * Faster animation                 C3  Decrease value of EVERY_N_MILLISECONDS() for the current (fixed) display mode.
 * Toggle direction                 C4  Toggle direction of some sequences. This is saved in EEPROM however very few demos use it.
 * 
 * Enable/disable glitter           D1  Toggles glitter. This is saved in EEPROM.
 * Previous display mode            D2  Also disables demo mode.
 * Next display mode                D3  Also disables demo mode.
 * Save Current mode to EEPROM      D4  This will be the startup mode, and disables demo mode temporarily (if it was enabled).
 * 
 * Decrease maxvol                  E1  Which increases peak detection sensitivity. This is saved in EEPROM.
 * Shorter mesh delay               E2  Decrease mesh delay by 100ms before starting (using white LED's), only if strand is active (with the Select Arduino command). This is saved in EEPROM. Press B1 again or A3 to reboot when done.
 * Longer mesh delay                E3  Increase mesh delay by 100ms before starting (using white LED's), only if strand is active (with the Select Arduino command). This is saved in EEPROM. Press B1 again or A3 to reboot when done.
 * Increase maxvol                  E4  Which decreases peak detection sensitivity. This is saved in EEPROM.
 * 
 * Decrease noise squelch           F1  Allows more ambient noise is displayed. This is saved in EEPROM.
 * Select previous palette          F2  Stop palette rotation and select previous palette immediately.
 * Select next palette              F3  Stop palette rotation and select next palette immediately.
 * Increase noise squelch           F4  Increases noise squelch, so that ambient noise = 0. This is saved in EEPROM.
 * 
 */


/*------------------------------------------------------------------------------------------
--------------------------------------- Start of variables ---------------------------------
------------------------------------------------------------------------------------------*/


#define qsubd(x, b)  ((x>b)?wavebright:0)                     // A digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Unsigned subtraction macro. if result <0, then => 0.

#define NOTASOUND_VERSION 110                                 // Just a continuation of notamesh, previously seirlight and previous to that was irlight, then aalight and then atlight. Turtles. . . 

#include "FastLED.h"                                          // https://github.com/FastLED/FastLED
#include "IRLremote.h"                                        // https://github.com/NicoHood/IRLremote
#include "EEPROM.h"                                           // This is included with base install

#include "commands.h"                                         // The IR commands transmitted from the keypad.

#if IRL_VERSION < 202
#error "Requires IRLRemote 2.0.2 or later. Check github for latest code."
#endif

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later. Check github for latest code."
#endif

#define pinIR 2                                               // Choose a valid PinInterrupt pin of your Arduino board for IR operations. In this case, D2.
#define IRL_BLOCKING true
uint16_t IRAddress;
uint8_t IRCommand;

CNec IRLremote;


// Fixed definitions cannot change on the fly.
#define LED_DT 4                                             // Serial data pin for all strands
//#define LED_CK 11                                             // Serial clock pin for WS2801 or APA102
#define COLOR_ORDER RGB                                       // It's GRB for WS2812
#define LED_TYPE WS2811                                       // Alternatively WS2801, or WS2812
#define MAX_LEDS 50                                       // Maximum number of LED's defined (at compile time).

// Fixed sound hardware definitions cannot change on the fly.
#define MIC_PIN    1                                          // Microphone on A1.

// Initialize changeable global variables.
uint8_t NUM_LEDS;                                             // Number of LED's we're actually using, and we can change this only the fly for the strand length.

uint8_t max_bright = 128;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[MAX_LEDS];                                   // Initialize our LED array. We'll be using less in operation.

CRGBPalette16 currentPalette;                                 // Use palettes instead of direct CHSV or CRGB assignments.
CRGBPalette16 targetPalette;                                  // Also support smooth palette transitioning.

TBlendType currentBlending = LINEARBLEND;                     // NOBLEND or LINEARBLEND for palettes


int matrice[6][8] = {                                                                 //Matrice poisson.
   
  { 0,  1,  2,  3,  4,  5,  6,  7},                                 //c??t?? droit
  {15, 14, 13, 12, 11, 10,  9,  8},
  {16, 17, 18, 19, 20, 21, 22, 23},
  {48, 47, 46, 45, 44, 43, 42, 41},                                 //c??t?? gauche
  {33, 34, 35, 36, 37, 38, 39, 40},
  {32, 31, 30, 29, 28, 27, 26, 25},                                  
 
};

extern const TProgmemRGBGradientPalettePtr GradientPalettes[]; // These are for the fixed palettes in gradient_palettes.h
extern const uint8_t GradientPaletteCount;                     // Total number of fixed palettes to display.
uint8_t CurrentPaletteNumber = 0;                              // Current palette number from the 'playlist' of color palettes
uint8_t currentPatternIndex = 0;                               // Index number of which pattern is current


// EEPROM location definitions.
#define ISINIT    0                                           // EEPROM location used to verify that this Arduino has been initialized
#define STARTMODE 1                                           // EEPROM location for the starting mode.
#define STRANDLEN 2                                           // EEPROM location for the actual Length of the strand, which is < MAX_LEDS
#define STRANDEL  3                                           // EEPROM location for the mesh delay value.
#define GLIT      4                                           // EEPROM location for glitter.
#define BRIT      5                                           // EEPROM location is brightness.
#define SQU       6                                           // EEPROM location of squelch.
#define MXV       7                                           // EEPROM location of maxvol.
#define SPED      8                                           // EEPROM location of speed.
#define DIRN      9                                           // EEPROM location of direction.
#define STARTPAL  10                                          // EEPROM location of current palette. If !0, then rotation = 0.

#define INITVAL   0                                        // If this is the value in ISINIT, then the Arduino has been initialized. Change to completely reset your Arduino.

#define INITBRIT 128                                          // Initial max_bright.
#define INITDEL  0                                            // Starting mesh delay value of the strand in milliseconds. Utilis?? pour ??chelonner le volume plut??t...
#define INITDIRN 1                                            // Initial thisdir value.
#define INITGLIT 0                                            // Glitter is off by default.
#define INITLEN  48                                           // Start length is 48 LED's.
#define INITMAX  20                                           // Starting maxvol value.
#define INITMODE 2                                            // Startmode is 15, which is a truc pour v??rifier que ??a va.
#define INITPAL  0                                            // Starting palette number.
#define INITSQU  20                                           // Starting squelch value.
#define INITSPED 0                                            // Initial thisdelay value.

const uint32_t STRANDID = IR_C1;                              // This is the ID button of the strand and should be unique for each strand in a series (if you want them to be different).
bool strandActive = 0;                                        // Used for configuration changes only. 0=inactive, 1=active. Must be activated by button press of B1, followed by C1 (or the appropriate STRANDID button).
bool strandFlag = 0;                                          // Flag to let us know if we're changing the active strand.

uint16_t meshdelay;                                           // Timer for the notasound. Works with INITDEL. Pas utilis??, on utiise les touches pour ??chelonner le volume plut??t.
uint16_t scalvol;                                             // R??glage de la sensibilit?? du micro...

uint8_t ledMode = 2;                                          // Starting mode is typically 0. Change INITMODE if you want a different starting mode.
uint8_t demorun = 0;                                          // 0 = regular mode, 1 = demo mode, 2 = shuffle mode.
uint8_t maxMode = 16;                                         // Maximum number of modes.
uint8_t demotime = 30;                                        // Set the length of the demo timer.


// Global timer value
uint16_t loops = 0;                                           // Our loops per second counter for showfps().


// Global variable(s) used by other routines.
uint8_t sampleavg = 0;                                        // Average of the last NSAMPLES samples.
uint8_t    sample = 10;                                       // Dampened 'sample' value from our twitchy microphone.
int8_t   squelch;                                             // Noise squelching with a non-zero default value, which we get from EEPROM.
int8_t  maxvol;                                               // Maxvol is used for peak detection sensitivity.
bool   samplepeak = 0;                                        // Peak detection, which is set by soundmems, but reset by the display routines.
//float beat=0;                                               // D??tection BPM 

// Reko Merio's global variables
const int maxBeats = 10;                                      // Min is 2 and value has to be divisible by two.
const int maxBubbles = 5; //NUM_LEDS / 3;                     // Decrease if there is too much action going on.
const int maxTrails = 5;                                      // Maximum number of trails.

// IR changeable variables
uint8_t palchg = 3;                                           // 0=no change, 1=similar, 2=random
uint8_t  thisdelay;                                           // Standard delay is initialized by EEPROM.
uint8_t    glitter;                                           // Glitter flag is initialized by EEPROM.
int8_t     thisdir;                                           // Standard direction is either -1 or 1. Used as a multiplier rather than boolean and is initialized by EEPROM.

//// 20 - 200hz Single Pole Bandpass IIR Filter
//float bassFilter(float sampli) {                                                // Je tente un filtre passe bas en trois ??tapes voir ce que ??a donne : En fait pas grand chose, le sampling rate est trop bas.
//    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
//    xv[0] = xv[1]; xv[1] = xv[2]; 
//    xv[2] = (sampli) / 3.f; // change here to values close to 2, to adapt for stronger or weeker sources of line level audio  
//    
//
//    yv[0] = yv[1]; yv[1] = yv[2]; 
//    yv[2] = (xv[2] - xv[0])
//        + (-0.7960060012f * yv[0]) + (1.7903124146f * yv[1]);
//    return yv[2];
//}
//
//// 10hz Single Pole Lowpass IIR Filter
//float envelopeFilter(float sampli) { //10hz low pass
//    static float xv[2] = {0,0}, yv[2] = {0,0};
//    xv[0] = xv[1]; 
//    xv[1] = sampli / 160.f;
//    yv[0] = yv[1]; 
//    yv[1] = (xv[0] + xv[1]) + (0.9875119299f * yv[0]);
//    return yv[1];
//}
//
//// 1.7 - 3.0hz Single Pole Bandpass IIR Filter
//float beatFilter(float sampli) {
//    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
//    xv[0] = xv[1]; xv[1] = xv[2]; 
//    xv[2] = sampli / 7.015f;
//    yv[0] = yv[1]; yv[1] = yv[2]; 
//    yv[2] = (xv[2] - xv[0])
//        + (-0.7169861741f * yv[0]) + (1.4453653501f * yv[1]);
//    return yv[2];
//}

// Display functions -----------------------------------------------------------------------

// Support functions
#include "soundmems.h"                                        // Sound reactive routines.
#include "structs.h"                                          // Reko Merio's structures.
#include "support.h"                                          // Support routines, such as showfps, glitter and routines to move up/down strand.
#include "gradient_palettes.h"                                // Using fixed gradient palettes rather than random ones.

//Reko Merio's global structure definitions
Bubble bubble[maxBubbles];
Bubble trail[maxTrails];


// Non sound reactive routine(s)
#include "noisepal.h"         //sorte de lava lampe. Modifi?? pour coller en 2D, ok

//// Main sound reactive routines
//
#include "fillnoise.h"      // Center to edges with base color and twinkle. Modifi?? pour la 2D, quelques buguitos mais rend plut??t OK.
#include "jugglep.h"        // Long line of sinewaves Pas modifi?? mais plut??t r??actif m??me si les couleurs sont pas toppes.
#include "ripple.h"         // Juggle with twinkles Pas modifi?? mais plut??t r??actif 
#include "pixel.h"          // Long line of colours Pas modifi??, pas foufou.
#include "matrix.h"         // Start to end with twinkles Pas fini mais OK
#include "onesine.h"        // Long line of shortlines Bizarre mais sympa
#include "fire.h"           // Compl??tement transform??, sert ?? la calibration...
#include "rainbowpeak.h"    // Long line of short lines with twinkles Pas modifi?? mais nickel comme ??a.
#include "firewide.h"       // Compl??tement transform??, c'est le motif1 de l'ann??e derni??re
#include "noisewide.h"      // Center to edges
#include "plasma.h"         // Long line of short lines Bidouill??, sympa mais pas nickel...
#include "besin.h"          // center to edges with black ??a serait bien que ??a fonctionne de l'int??rieur vers l'ext??rieur mais j'y arrive pas.
//#include "noisewide.h"      // Long line
#include "myvumeter.h"      // My own vu meter Pas modifi??, un peu spiraleux mais sympa.
#include "sinephase.h"      // Changing phases of sine waves Bidouill?? du centre vers l'ext??rieur. OK !

// Reko Merio display routines
#include "bubbles.h"        // Bubbles .
#include "trails.h"         // Trails Pas modifi??, ok

//DJ Mitchou Pitchou routines
//#include "ronds.h"          // Enfin du 2D ! Fait planter la d??tection de son :(


/*------------------------------------------------------------------------------------------
--------------------------------------- Start of code --------------------------------------
------------------------------------------------------------------------------------------*/


void setup() {

  Serial.begin(115200);                                                           // Setup serial baud rate

  Serial.println(F(" ")); Serial.println(F("---SETTING UP notasound---"));

  analogReference(DEFAULT);                                                      // Comment out this line for 3.3V Arduino's, ie. Flora, etc or if powering microphone with 5V.
  
  delay(1000);                                                                    // Slow startup so we can re-upload in the case of errors.

  if (!IRLremote.begin(pinIR))
    Serial.println(F("You did not choose a valid pin."));
  
//    LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER >(leds, MAX_LEDS);       // APA102 or WS2801 4 pin definition.
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER >(leds, MAX_LEDS);                   // WS2812 3 pin definition.
  
//  set_max_power_in_volts_and_milliamps(5, 200);                                   // 5V, 200mA maximum power draw.
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 200); 


// Setup the ADC on a Nano or similar AVR for polled 10 bit sampling on analog pin 5 at 9.6KHz.
// You can remove all of this and just use the analogRead(MIC_PIN) in soundmems.h if you want.

  cli();                                  // Disable interrupts.
  ADCSRA = 0;                             // Clear this register.
  ADCSRB = 0;                             // Ditto.
  ADMUX = 0;                              // Ditto.
  ADMUX |= (MIC_PIN & 0x07);              // Set A5 analog input pin.
  ADMUX |= (1 << REFS0);                  // Set reference voltage  (analog reference(external), or using 3.3V microphone on 5V Arduino.
                                          // Set that to 1 if using 5V microphone or 3.3V Arduino.
//  ADMUX |= (1 << ADLAR);                  // Left justify to get 8 bits of data.                                          
  ADMUX |= (0 << ADLAR);                  // Right justify to get full 10 A/D bits.

//  ADCSRA |= bit (ADPS0) | bit (ADPS2);                //  32 scaling or 38.5 KHz sampling
//  ADCSRA |= bit (ADPS1) | bit (ADPS2);                //  Set ADC clock with 64 prescaler where 16mHz/64=250kHz and 250khz/13 instruction cycles = 19.2khz sampling.
  ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);    // 128 prescaler with 9.6 KHz sampling
  
  ADCSRA |= (1 << ADATE);                 // Enable auto trigger.
//  ADCSRA |= (1 << ADIE);                  // Enable interrupts when measurement complete (if using ISR method). If commented out, then we're using polling.
  ADCSRA |= (1 << ADEN);                  // Enable ADC.
  ADCSRA |= (1 << ADSC);                  // Start ADC measurements.
  sei();                                  // Re-enable interrupts.

  Serial.println(F("---SOUND COMPLETE---"));


  if (EEPROM.read(ISINIT) != INITVAL) {                                           // Check to see if Arduino has been initialized, and if not, do so.
    EEPROM.write(STARTMODE, INITMODE);                                            // Initialize the starting mode to 0.
    EEPROM.write(STRANDLEN, INITLEN);                                             // Initialize the starting length to 20 LED's.
    EEPROM.write(ISINIT, INITVAL);                                                // Initialize the starting value (so we know it's initialized) to INITVAL.
    EEPROM.write(STRANDEL, INITDEL);                                              // Initialize the notasound delay to 0.
    EEPROM.write(SQU, INITSQU);                                                   // Initialize the squelch value.
    EEPROM.write(MXV, INITMAX);                                                   // Initialize maxvol value.
    EEPROM.write(GLIT, INITGLIT);                                                 // Initialize glitter value.
    EEPROM.write(BRIT, INITBRIT);                                                 // Initialize the brightness value.
    EEPROM.write(SPED, INITSPED);                                                 // Initial thisdelay value.
    EEPROM.write(DIRN, INITDIRN);                                                 // Initial thisdir value.
    EEPROM.write(STARTPAL, INITPAL);                                              // Initialize the palette to 0.
    Serial.println(F("Cold reset."));
  }

  ledMode = EEPROM.read(STARTMODE);                                               // Location 0 is the starting mode.
  NUM_LEDS = EEPROM.read(STRANDLEN);                                              // Need to ensure NUM_LEDS < MAX_LEDS elsewhere.
  //meshdelay = EEPROM.read(STRANDEL);                                              // This is our notasound delay for cool delays across strands.
  scalvol = EEPROM.read(STRANDEL);
  squelch = EEPROM.read(SQU);                                                     // notasound squelch is stored in EEPROM.
  maxvol = EEPROM.read(MXV);                                                      // notasound maxvol for peak detection in EEPROM.
  glitter = EEPROM.read(GLIT);                                                    // notasound glitter.
  max_bright = EEPROM.read(BRIT);                                                 // max_bright value.
  thisdelay = EEPROM.read(SPED);                                                  // thisdelay value.
  thisdir = EEPROM.read(DIRN);                                                    // thisdir value.

  if (EEPROM.read(STARTPAL) != 0) {CurrentPaletteNumber = EEPROM.read(STARTPAL); palchg = 0;}

  Serial.println(F("---EEPROM COMPLETE---"));
 
  //Serial.print(F("Initial mesh delay: ")); Serial.print(meshdelay*100); Serial.println(F("ms delay"));
  Serial.print(F("Initial scalvol: ")); Serial.print(scalvol*100); Serial.println(F("facteur"));
  Serial.print(F("Initial strand length: ")); Serial.print(NUM_LEDS); Serial.println(F(" LEDs"));
  Serial.print(F("Strand ID: ")); Serial.println(STRANDID);
  Serial.print(F("Squelch: ")); Serial.println(squelch);
  Serial.print(F("Maxvol: ")); Serial.println(maxvol);
  Serial.print(F("Glitter: ")); Serial.println(glitter);
  Serial.print(F("Brightness: ")); Serial.println(max_bright);
  Serial.print(F("Delay: ")); Serial.println(thisdelay);
  Serial.print(F("Direction: ")); Serial.println(thisdir);
  Serial.print(F("Palette: ")); Serial.println(EEPROM.read(STARTPAL));

  currentPalette = CRGBPalette16(CRGB::Black);
  targetPalette = (GradientPalettes[0]);

  LEDS.setBrightness(max_bright);                                                 // Set the generic maximum brightness value.
  
  strobe_mode(ledMode, 1);                                                        // Initialize the first sequence.

  Serial.println(F("---SETUP COMPLETE---")); Serial.println(F(" "));

} // setup()



//------------------MAIN LOOP---------------------------------------------------------------
void loop() {

  soundmems();                                                                // Sample sound, measure averages and detect peak.
  
  getirl();                                                                   // Read a command from the IR LED and process command as required.
  
  demo_check();                                                               // If we're in demo mode, check the timer to see if we need to increase the strobe_mode value.

  EVERY_N_MILLISECONDS(50) {                                                  // Smooth palette transitioning runs continuously.
    uint8_t maxChanges = 24; 
      nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  
  }

  EVERY_N_SECONDS(5) {                                                        // If selected, change the target palette to a random one every 5 seconds.
    if (palchg==3) {
      CurrentPaletteNumber = addmod8(CurrentPaletteNumber, 1, GradientPaletteCount);
    }
    targetPalette = GradientPalettes[CurrentPaletteNumber];                  // We're just ensuring that the targetPalette WILL be assigned.
  }

  EVERY_N_MILLIS_I(thistimer, thisdelay) {                                    // Sets the original delay time.
    thistimer.setPeriod(thisdelay);                                           // This is how you update the delay value on the fly.
    strobe_mode(ledMode, 0);                                                  // Strobe to display the current sequence, but don't initialize the variables, so mc=0;
  }

  if(glitter) addGlitter(sampleavg/2);                                        // If the glitter flag is set, let's add some.

  if (!IRLremote.receiving()) {
    FastLED.show();                                                           // Power managed display of LED's.
  }

  
} // loop()



//-------------------OTHER ROUTINES----------------------------------------------------------
void strobe_mode(uint8_t newMode, bool mc){                   // mc stands for 'Mode Change', where mc = 0 is to display the routine, while mc = 1 is to initialize and then display the routine.

  if(mc) {
    fill_solid(leds,NUM_LEDS,CRGB(0,0,0));                    // Clean up the array for the first time through. Don't show display though, so you may have a smooth transition.
    Serial.print(F("Mode: ")); 
    Serial.println(newMode);

  }

  if (!strandActive) {                                          // Stops the display sequence if we're updating the EEPROM in ACTIVE mode.
    switch (newMode) {                                          // If first time through a new mode, then initialize the variables for a given display, otherwise, just call the routine.
      case   0: if(mc) {thisdelay=20;} noisepal(); break;                                 // Change mode 0 to a generic non-reactive noise routine.
      case   1: if(mc) {thisdelay=20;} ripple(); break;                                   // samplepeak
      case   2: if(mc) {thisdelay=40;} fillnoise(); break;                                // sampleavg
      case   3: if(mc) {thisdelay=40;} bubbles(); break;                                  // samplepeak
      case   4: if(mc) {thisdelay= 0;} pixel(); break;                                    // sample
      case   5: if(mc) {thisdelay=30;} onesine(); break;                                  // sampleavg
      case   6: if(mc) {thisdelay=10;} rainbowpeak(); break;                              // samplepeak
      case   7: if(mc) {thisdelay=10;} noisewide(); break;                                // sampleavg
      case   8: if(mc) {thisdelay=30;} myvumeter(); break;                                // sampleavg
      case   9: if(mc) {thisdelay=10;} jugglep(); break;                                  // sampleavg
      case  10: if(mc) {thisdelay=10;} firewide(); break;                                 // sampleavg
      case  11: if(mc) {thisdelay=40;} trails(); break;                                   // samplepeak
      case  12: if(mc) {thisdelay=20;} plasma(); break;                                   // sampleavg
      case  13: if(mc) {thisdelay=30;} besin(); break;                                    // sampleavg
      case  14: if(mc) {thisdelay=40;} matrix(); break;                                   // sample
      case  15: if(mc) {thisdelay= 0;} fire(); break;                                     // sampleavg
      case  16: if(mc) {thisdelay=10;} sinephase(); break;                                // sampleavg
//      case  17: if(mc) {thisdelay=10;} ronds();break;                                     // samplepeak
      default: break;
    } // switch newMode
  } // !strandActive

} // strobe_mode()



void demo_check(){                                                // Are we in demo mode or not, and if so, change the routine every 'demotime' seconds.
  
  if(demorun) {                                                   // Is the demo flag set? If so, let's cycle through them.
    uint8_t secondHand = (millis() / 1000) % (maxMode*demotime);  // Adjust for total time of the loop, based on total number of available modes.
    static uint8_t lastSecond = 99;                               // Static variable, means it's only defined once. This is our 'debounce' variable.
    if (lastSecond != secondHand) {                               // Debounce to make sure we're not repeating an assignment.
      lastSecond = secondHand;
        if(secondHand%demotime==0) {                              // Every 10 seconds.
          if(demorun == 2) {
            ledMode = random8(0,maxMode);                         // This is shuffle mode, which I never use.
          } else {
            ledMode = secondHand/demotime;
          } // if secondHand
          meshwait();
          strobe_mode(ledMode,1);                                 // Does NOT reset to 0.
      } // if secondHand
    } // if lastSecond
  } // if demorun
  
} // demo_check()



//----------------- IR Receiver and Button Command Processing ---------------------------------------------

void getirl() {                                                   // This is the IR function that gets the value and selects/performs a command.
  
  if (IRLremote.available()) {
  
    auto irdata = IRLremote.read();           // Get the new data from the remote.
    IRAddress = irdata.address;             // Do this if we want to swap out values.
    IRCommand = irdata.command;             // Do this if we want to swap out values.

    if(strandFlag == 1) set_strand();       // Set the strand length
    
//    Serial.print(F("Address: "));           // Print the protocol data. Note that there's also 65535, which we don't use.
//    Serial.println(irdata.address);
//    Serial.print(F("Command: "));
//    Serial.println(irdata.command);   
//    Serial.println();
    

    if (IRAddress == IR_ADD) {     
      switch(IRCommand) {

        case IR_A1:  max_bright=min(max_bright*2,255); EEPROM.write(BRIT, max_bright); Serial.print(F("Bright: ")); Serial.println(max_bright); LEDS.setBrightness(max_bright); break;  //a1 - Increase max bright
        case IR_A2:  max_bright=max(max_bright/2,1); EEPROM.write(BRIT, max_bright); Serial.print(F("Bright: ")); Serial.println(max_bright); LEDS.setBrightness(max_bright); break;    //a2 - Decrease max bright
        case IR_A3:  fill_solid(leds,NUM_LEDS,CRGB(0,0,0)); FastLED.show(); Serial.println(F("Rebooting . . ")); FastLED.delay(100); bootme(); break;                                   //a3 - Change to mode 0, display and reboot
        case IR_A4:  demorun = 1; if(demorun) {Serial.println(F("Demo mode")); meshwait();} else {Serial.println(F("Not demo mode"));} break;                                           //a4 - Enable (not toggle) demo mode
  
        case IR_B1:  Serial.println(F("Activate request")); set_strand(); break;                                                                            //b1 - Set Strand Active or Inactive for EEPROM programming.
        case IR_B2:  if (strandActive==1) set_strandlen(); break;                                                                                           //b2 - Decrease # of LED's and write to EEPROM
        case IR_B3:  if (strandActive==1) set_strandlen(); break;                                                                                           //b3 - Increase # of LED's and write to EEPROM
        case IR_B4:  palchg = 3; Serial.print(F("Continuous palette change: "));  Serial.println(CurrentPaletteNumber); EEPROM.write(STARTPAL, 0); break;   //b4 - Continuous palette change.
  
        case IR_C1:  palchg = 0; Serial.print(F("Stop and select current Palette: ")); EEPROM.write(STARTPAL, CurrentPaletteNumber); Serial.println(CurrentPaletteNumber); break; //c1 - Stop and select current palette.
        case IR_C2:  thisdelay++; EEPROM.write(SPED, thisdelay); Serial.print(F("Delay: ")); Serial.println(thisdelay); break;                                                    //c2 - Slow down the sequence as much as you want.
        case IR_C3:  thisdelay--; if(thisdelay >30000) thisdelay = 0; EEPROM.write(SPED, thisdelay); Serial.print(F("Delay: ")); Serial.println(thisdelay); break;                //c3 - Speed up the sequence, but don't go too far.
        case IR_C4:  thisdir = thisdir*-1; EEPROM.write(DIRN, thisdir); Serial.print(F("thisdir = ")); Serial.println(thisdir);  break;                                           //c4 - Change the direction of the LEDs.
  
        case IR_D1:  glitter = !glitter; EEPROM.write(GLIT, glitter); Serial.print(F("Glitter is: ")); Serial.println(glitter);   break;    //d1 - Toggle glitter.
        case IR_D2:  demorun = 0; ledMode=(ledMode-1); if (ledMode==255) ledMode=maxMode; meshwait(); strobe_mode(ledMode,1); break;        //d2 - Stop demo and display previous mode.
        case IR_D3:  demorun = 0; ledMode=(ledMode+1)%(maxMode+1); meshwait(); strobe_mode(ledMode,1); break;                               //d3 - stop demo and display next mode.
        case IR_D4:  EEPROM.write(STARTMODE,ledMode); Serial.print(F("Writing startup mode: ")); Serial.println(ledMode);  break;           //d4 - Save current mode as startup mode.


        case IR_E1:  maxvol--; if(maxvol <0) maxvol = 0; EEPROM.write(MXV, maxvol);Serial.print(F("Maxvol: ")); Serial.println(maxvol);  break;   //e1 -  Reduce maxvol for more sensitive peak detection
//        case IR_E2:  if (strandActive==1) set_meshdel(); break;                                                                                   //e2 - Shorter mesh delay by 100ms
//        case IR_E3:  if (strandActive==1) set_meshdel(); break;                                                                                   //e3 - Longer mesh delay by 100ms
        case IR_E2:  scalvol--; if(scalvol<1) scalvol = 1; EEPROM.write(STRANDEL,scalvol);Serial.print(F("scalvol:"));Serial.println(scalvol); break;    //e2 - On r??duit la variable scalvol
        case IR_E3:  scalvol++; if(scalvol>50) scalvol = 0; EEPROM.write(STRANDEL,scalvol);Serial.print(F("scalvol:"));Serial.println(scalvol); break;    //e3 - On rallonge la variable scalvol
        case IR_E4:  maxvol++; EEPROM.write(MXV, maxvol);Serial.print(F("Maxvol: ")); Serial.println(maxvol); break;                              //e4 -  Increase maxvol for less sensitive peak detection

        case IR_F1:  squelch--; if(squelch <0) squelch = 0; EEPROM.write(SQU, squelch);Serial.print(F("Squelch: ")); Serial.println(squelch);  break;  //f1 -  Reduce squelch value
//        case IR_F1:  palchg = 0; Serial.print(F("Stop and select current Palette ")); Serial.println(CurrentPaletteNumber); break;             //f1 - Stop and select current Palette
        case IR_F2:  palchg = 1; Serial.print(F("Stop and select previous Palette ")); CurrentPaletteNumber -= 1; if(CurrentPaletteNumber == 255) CurrentPaletteNumber = GradientPaletteCount; Serial.println(CurrentPaletteNumber); currentPalette = (GradientPalettes[CurrentPaletteNumber]); break;    //f2 - Stop and select previous Palette
        case IR_F3:  palchg = 2; Serial.print(F("Stop and select next Palette ")); CurrentPaletteNumber = addmod8( CurrentPaletteNumber, 1, GradientPaletteCount);  Serial.println(CurrentPaletteNumber);  currentPalette = (GradientPalettes[CurrentPaletteNumber]); break;             //f3 - Stop and select next Palette
//        case IR_F4:  palchg = 3; Serial.print(F("Continuous palette change "));  Serial.println(CurrentPaletteNumber); break;                 //f4 - Continuous palette change
        case IR_F4:  squelch++; EEPROM.write(SQU, squelch); Serial.print(F("Squelch: ")); Serial.println(squelch); break;                    //f4 - Increase squelch value.

        default:     break;                                     // We could do something by default.

      } // switch IRCommand
    } // if IR_ADD
  } // if IRLRemote

} // getirl()



void bootme() {                                                 // This is used to reset all the Arduinos so that their millis() counters are all in sync.

  if (millis() < 5000) {
    Serial.println(F("Factory Reset."));                        // If we reset within 2 seconds of startup, then it means factory reset.
    EEPROM.write(ISINIT, 0);
    delay(200);
  }

  asm volatile("jmp 0");
  
} // bootme()



void meshwait() {                                               // After we press a mode key, we need to wait a bit for the sequence to start.

//  Serial.print(F("Mesh delay: ")); Serial.print(meshdelay*100); Serial.println(F("ms delay."));
  FastLED.delay(meshdelay*100);                                 // Here's our notasound wait upon keypress. I'm so sorry there's a delay statement here. At least it's only used upon mode change keypress. Makes life a LOT simpler.

} // meshwait()



void set_strand() {                                             // Setting the active strand. This logic is not great, so we have to reboot when done.

  if (IRAddress == IR_ADD && IRCommand == IR_B1) {
    strandFlag = !strandFlag;
    if (strandFlag == false) {IRCommand = 255; strandActive = 0; Serial.println(F("INACTIVE"));}
  }

  if (strandFlag == true && IRAddress == IR_ADD && IRCommand == STRANDID) {
    strandActive = 1; Serial.println(F("ACTIVE"));
  }

} // set_strand()



void set_strandlen() {                                                  // Setting our strand length with button presses.

  Serial.println(F("Setting strand length."));
  
  if(strandActive == 1) {                                               // Only do this if the strand is active.
    demorun = 0;                                                        // First we disable the demo mode.
    fill_solid(leds,MAX_LEDS, CRGB::Black);                             // Let's make it black manually.
    
    if (IRCommand == IR_B2) {
      NUM_LEDS--; if (NUM_LEDS >255) NUM_LEDS=0;                        // Don't be stupid with our strand length selection.
    } else {
     NUM_LEDS++;  if (NUM_LEDS >= MAX_LEDS) NUM_LEDS=MAX_LEDS-1;        // Again, don't be stupid with our strand length selection.
    }
    fill_solid(leds,NUM_LEDS,CRGB(64, 64, 64));                         // Turn on the number of LEDs that we have selected as our length.
    EEPROM.write(STRANDLEN,NUM_LEDS);                                   // Write that value to EEPROM.
    Serial.print(F("Writing IR: ")); Serial.print(NUM_LEDS); Serial.println(F(" LEDs"));
  } // if strandActive
  
} // set_strandlen()



void set_meshdel() {                                                    // Setting our notasound delay for whatever strands are powered up. Pas utilis?? ici.

  if(strandActive == 1) {                                               // Only do this if the strand is active.
    demorun = 0;                                                        // First we disable the demo mode.
    ledMode = 0;                                                        // And set to mode 0 (black).
    fill_solid(leds,MAX_LEDS, CRGB::Black);                             // Let's make it black manually.
    
    if (IRCommand == IR_E2) {                                           // Which button did we press (either E2 or E3).
      meshdelay = meshdelay - 1;
      if (meshdelay >10000) meshdelay = 0;                              // Again, don't be stupid with our buttons.
    } else {
     meshdelay = meshdelay + 1;                                         // Increase the delay as much as you want. . .
    } // if IRCommand
    
    fill_solid(leds,meshdelay,CRGB(64, 64, 64));                        // Turn on the number of LED's that we have selected (100ms is 1 LED)
    EEPROM.write(STRANDEL,meshdelay);                                   // Write out the delay to EEPROM.
    Serial.print(F("Writing IR: ")); Serial.print(meshdelay*100); Serial.println(F("ms delay."));
  } // if strandActive
  
} // set_meshdel()
