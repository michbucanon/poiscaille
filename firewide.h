#ifndef FIREWIDE_H
#define FIREWIDE_H

  uint8_t bright0 = 0,
        bright1 = 0,
        bright2 = 0;

void firewide() {         

  uint8_t vit = 600/thisdelay;

  bright0 = beatsin8(vit, 0, sampleavg, 2 * vit);
  bright1 = beatsin8(vit, 0, sampleavg, 0);
  bright2 = beatsin8(vit, 0, sampleavg, 2 * vit);


  if (bright0<10) bright0=0;
  if (bright1<10) bright1=0;
  if (bright2<10) bright2=0;

//  Serial.print(bright1);
//  Serial.print(" ");
//  Serial.print(bright2);
//  Serial.print(" ");
//  Serial.println(bright3);



  for (int i = 0; i < 8; i++) {
    showLed(i, 2, beatsin8(vit,0,255)+10*i, bright0);
    showLed(i, 1, beatsin8(vit,0,255,vit/2)+10*i, bright1);
    showLed(i, 0, beatsin8(vit,0,255,vit)+10*i, bright2);
    showLed(i, 5, beatsin8(vit,0,255)+10*i, bright0);
    showLed(i, 4, beatsin8(vit,0,255,vit/2)+10*i, bright1);
    showLed(i, 3, beatsin8(vit,0,255,vit)+10*i, bright2);

  }


  fadeToBlackBy(leds, NUM_LEDS, 2000 / vit);


} // firewide()

#endif
