/*
  Скетч к проекту "Эффектный светильник"
  Страница проекта (схемы, описания): https://alexgyver.ru/gyverlight/
  Исходники на GitHub: https://github.com/AlexGyver/gyverlight/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  https://AlexGyver.ru/
*/

/*
   Управление кнопкой/сенсором
  - Удержание - яркость
  - 1х тап - вкл/выкл
  - 2х тап - переключ режима
  - 3х тап - вкл/выкл белый свет
  - 4х тап - старт/стоп авто смены режимов
*/

/*
   Версия 1.3 - пофикшен баг с fillAll
*/

// ************************** НАСТРОЙКИ ***********************
#define CURRENT_LIMIT 800  // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define AUTOPLAY_TIME 120    // время между сменой режимов в секундах

#define NUM_LEDS 8         // количсетво светодиодов в одном отрезке ленты
#define NUM_STRIPS 4        // количество отрезков ленты (в параллели)
#define LED_PIN 2           // пин ленты
#define BTN_PIN 10           // пин кнопки/сенсора
#define MIN_BRIGHTNESS 5  // минимальная яркость при ручной настройке
#define BRIGHTNESS 50      // начальная яркость


// ************************** ДЛЯ РАЗРАБОТЧИКОВ ***********************

#define MODES_AMOUNT 5    // общее кол-во цветных режимов

#include "GyverButton.h"
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

#include <FastLED.h>
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;

#include "GyverTimer.h"
GTimer_ms effectTimer(80);
GTimer_ms autoplayTimer((long)AUTOPLAY_TIME * 1000);
GTimer_ms brightTimer(20);

int brightness = BRIGHTNESS;
int tempBrightness;
byte thisMode;

boolean loadingFlag = true;
boolean autoplay = false;
boolean powerDirection = true;
boolean powerActive = false;
boolean powerState = true;
boolean whiteMode = false;
boolean brightDirection = true;
boolean wasStep = false;


// залить все
void fillAll(CRGB newcolor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = newcolor;
  }
}

// функция получения цвета пикселя по его номеру
uint32_t getPixColor(int thisPixel) {
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}

void setup() {
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT / NUM_STRIPS);
  FastLED.setBrightness(brightness);
  FastLED.show();

  randomSeed(analogRead(0));
  touch.setTimeout(300);
  touch.setStepTimeout(50);
}

void loop() {
  touch.tick();
  if (touch.hasClicks()) {
    byte clicks = touch.getClicks();
    switch (clicks) {
      case 1:
        powerDirection = !powerDirection;
        powerActive = true;
        tempBrightness = brightness * !powerDirection;
        break;
      case 2: if (!whiteMode && !powerActive) {
          nextMode();
        }
        break;
      case 3: if (!powerActive) {
          whiteMode = !whiteMode;
          if (whiteMode) {
            effectTimer.stop();
            fillAll(CRGB::White);
            FastLED.show();
          } else {
            effectTimer.start();
          }
        }
        break;
      case 4: if (!whiteMode && !powerActive) autoplay = !autoplay;
        break;
      default:
        break;
    }
  }

  if (touch.isStep()) {
    if (!powerActive) {
      wasStep = true;
      if (brightDirection) {
        brightness += 1;
      } else {
        brightness -= 1;
      }
      brightness = constrain(brightness, MIN_BRIGHTNESS, 255);
      FastLED.setBrightness(brightness);
      FastLED.show();
    }
  }

  if (touch.isRelease()) {
    if (wasStep) {
      wasStep = false;
      brightDirection = !brightDirection;
    }
  }

  if (effectTimer.isReady() && powerState) {
    switch (thisMode) {
      case 0: fast_rainbow();
        break;
      case 1: rainbow();
        break;
      case 2: new_rainbow_loop();
        break;
      case 3: sparkles();
        break;
      case 4: lightBugs();
        break;
    }
    FastLED.show();
  }

  if (autoplayTimer.isReady() && autoplay) {    // таймер смены режима
    nextMode();
  }

  brightnessTick();
}

void nextMode() {
  thisMode++;
  if (thisMode >= MODES_AMOUNT) thisMode = 0;
  loadingFlag = true;
  FastLED.clear();
}

void brightnessTick() {
  if (powerActive) {
    if (brightTimer.isReady()) {
      if (powerDirection) {
        powerState = true;
        tempBrightness += 10;
        if (tempBrightness > brightness) {
          tempBrightness = brightness;
          powerActive = false;
        }
        FastLED.setBrightness(tempBrightness);
        FastLED.show();
      } else {
        tempBrightness -= 10;
        if (tempBrightness < 0) {
          tempBrightness = 0;
          powerActive = false;
          powerState = false;
        }
        FastLED.setBrightness(tempBrightness);
        FastLED.show();
      }
    }
  }
}

//===================================== ЭФФЕКТЫ ===============================================
//=============================================================================================


// ***************** Радуга быстрее и наоборот *****************

byte hue = 0;
void fast_rainbow() {
 hue +=  30;
  for (byte i = 0; i < NUM_LEDS; i++)
    leds[i] = CHSV((byte)(hue + i * float(255 - (255 / NUM_LEDS))), 255, 255);
}


// ***************** Радуга *****************

void rainbow() {
 hue += 1;
  for (byte i = 0; i < NUM_LEDS; i++)
    leds[i] = CHSV((byte)(hue + i * float(255 / NUM_LEDS)), 255, 255);
}


// ***************** Крутая плавная вращающаяся радуга (30)*****************

int ihue = 0;
void new_rainbow_loop() {
  ihue -= 1;
  fill_rainbow( leds, NUM_LEDS, ihue );
}


// ***************** Конфети (перенесено из Vadim) *****************

void sparkles() {
  byte thisNum = random(0, NUM_LEDS);
  if (getPixColor(thisNum) == 0)
    leds[thisNum] = CHSV(random(0, 255), 255, 255);
  for(int i = 0; i < NUM_LEDS; i++){
     CRGB c = (leds[i]);
     leds[i] = CRGB(constrain(c.r-random(3,10),0,255),constrain(c.g-random(3,10),0,255),constrain(c.b-random(3,10),0,255) );
  }
}


// ***************** Светлячки *****************

#define MAX_SPEED 15
#define BUGS_AMOUNT 7
int8_t speed[BUGS_AMOUNT];
int8_t pos[BUGS_AMOUNT];
CRGB bugColors[BUGS_AMOUNT];

void lightBugs() {
  if (loadingFlag) {
    loadingFlag = false;
    for (byte i = 0; i < BUGS_AMOUNT; i++) {
      bugColors[i] = CHSV(random(0, 9) * 28, 255, 255);
      pos[i] = random(0, NUM_LEDS);
      speed[i] += random(-5, 6);
    }
  }
 
  for (byte i = 0; i < BUGS_AMOUNT; i++) {
    if(random(5) == 1)speed[i] += random(-10, 10);
    if (speed[i] == 0) speed[i] += random(-1,1)*11;
    if(random(200)==3) bugColors[i] = CHSV(random(0, 9) * 28, 255, 255);
    if (abs(speed[i]) > MAX_SPEED) speed[i] = random(-5, 6);
    pos[i] += speed[i] / 10;
    if (pos[i] < 0) {
      pos[i] = 0;
      speed[i] = -speed[i]/1.5;
    }
    if (pos[i] > NUM_LEDS - 1) {
      pos[i] = NUM_LEDS - 1;
      speed[i] = -speed[i]/1.5;
    }

    leds[pos[i]].r = bugColors[i].r;
    leds[pos[i]].g = bugColors[i].g;
    leds[pos[i]].b = bugColors[i].b;
  }

  for(byte i = 0; i < NUM_LEDS; i++){
     if(i != pos[i]) leds[i] = CRGB(constrain(leds[i].r-40,0,255),constrain(leds[i].g-40,0,255),constrain(leds[i].b-40,0,255) );
  }
}
