// ****************************** безумие случайных цветов (14) ******************************

 // вспомогательные функции который пришлось перенести из сборника эффектов
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {
    r = i - 1;
  }
  else {
    r = NUM_LEDS - 1;
  }
  return r;
}

int ledsX[NUM_LEDS][3];

void copy_led_array() {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    ledsX[i][0] = leds[i].r;
    ledsX[i][1] = leds[i].g;
    ledsX[i][2] = leds[i].b;
  }
}  


void random_march() {                   //-m14-RANDOM MARCH CCW  (сама функция эффектов)
  copy_led_array();
  int iCCW;
  leds[0] = CHSV(random(0, 255), 255, 255);
  for (byte idex = 1; idex < NUM_LEDS ; idex++ ) {
    leds[idex] = CHSV(random(0, 255), 255, 255);
    iCCW = adjacent_ccw(idex);
    leds[idex].r = ledsX[iCCW][0];
    leds[idex].g = ledsX[iCCW][1];
    leds[idex].b = ledsX[iCCW][2];
  }
  delay(60);
}

// ****************************** крутая плавная вращающаяся радуга (30)******************************

int ihue = 0;
void new_rainbow_loop() {                      //-m88-RAINBOW FADE FROM FAST_SPI2
  ihue -= 1;
  fill_rainbow( leds, NUM_LEDS, ihue );
}

// ****************************** РАДУГА ******************************
byte hue = 0;
void rainbow() {
 hue += 2;
  for (byte i = 0; i < NUM_LEDS; i++)
    leds[i] = CHSV((byte)(hue + i * float(255 / NUM_LEDS)), 255, 255);
}

// ****************************** КОНФЕТТИ *******перенесено из Vadim***********************
void sparkles() {
  byte thisNum = random(0, NUM_LEDS);
  if (getPixColor(thisNum) == 0)
    leds[thisNum] = CHSV(random(0, 255), 255, 255);
  for(int i = 0; i < NUM_LEDS; i++){
     CRGB c = (leds[i]);
     leds[i] = CRGB(constrain(c.r-random(3,10),0,255),constrain(c.g-random(3,10),0,255),constrain(c.b-random(3,10),0,255) );
  }
}

//*****************RGB пропеллер (27) (вот именно он ночью и заглючил)********************

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS DELAY VAR
int thishue = 0;             //-FX LOOPS DELAY VAR
int thissat = 255;           //-FX LOOPS DELAY VAR
int idex2 = 0; 

void rgb_propeller() {                           //-m27-RGB PROPELLER
  idex2++;
  int ghue = (thishue + 80) % 255;
  int bhue = (thishue + 160) % 255;
  int N3  = int(NUM_LEDS / 3);
  int N6  = int(NUM_LEDS / 6);
  int N12 = int(NUM_LEDS / 12);
  for (int i = 0; i < N3; i++ ) {
    int j0 = (idex2 + i + NUM_LEDS - N12) % NUM_LEDS;
    int j1 = (j0 + N3) % NUM_LEDS;
    int j2 = (j1 + N3) % NUM_LEDS;
    leds[j0] = CHSV(thishue, thissat, 255);
    leds[j1] = CHSV(ghue, thissat, 255);
    leds[j2] = CHSV(bhue, thissat, 255);
  }
  delay(60);
}
