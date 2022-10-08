#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <FastLED.h>

FASTLED_USING_NAMESPACE

char auth[] = "zKpTxgmjwjoelU_jzHMy8Y53ewXO3oPz";
char ssid[] = "";
char pass[] = "";

#define DATA_PIN 3
#define NUM_LEDS 255
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define VAL 255
CRGB leds[NUM_LEDS];

int hue = 100;
int sat = 100;
int brightness = 100;

int state = 0;
int manual = 1;

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(100);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, colorFade };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
  Blynk.run();
  if(state && !manual) {
    gPatterns[gCurrentPatternNumber]();
    EVERY_N_MILLISECONDS( 20 ) gHue++;  // slowly cycle the "base color" through the rainbow
  }
  FastLED.show();
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

BLYNK_WRITE(V0) {
  state = param.asInt();
  if(!state) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
  else {
    for(int i = 0; i <= NUM_LEDS; i++) {
      leds[i].setHSV(hue, sat, VAL); 
    }
  }
  FastLED.show();
}

BLYNK_WRITE(V1) {
  hue = param.asInt();
  if(state && manual) {
    for(int i = 0; i <= NUM_LEDS; i++) {
      leds[i].setHSV(hue, sat, VAL); 
    }
    FastLED.show();
  }
}

BLYNK_WRITE(V2) {
  sat = param.asInt();
  if(state && manual) {
    for(int i = 0; i <= NUM_LEDS; i++) {
      leds[i].setHSV(hue, sat, VAL); 
    }
    FastLED.show();
  }
}

BLYNK_WRITE(V3) {
  brightness = param.asInt();
  FastLED.setBrightness(brightness);
  FastLED.show();
}

BLYNK_WRITE(V8) {
  manual = param.asInt();
  if(state && manual) {
    for(int i = 0; i <= NUM_LEDS; i++) {
      leds[i].setHSV(hue, sat, VAL); 
    }
  }
  FastLED.show();
}

BLYNK_WRITE(V4) {
  if(state && manual) {
    for(int i = 0; i <= NUM_LEDS; i++) {
      leds[i] = CRGB::Red; 
    }
    FastLED.show();
  }
}

BLYNK_WRITE(V5) {
  if(state && manual) {
    for(int i = 0; i <= NUM_LEDS; i++) {
      leds[i] = CRGB::Green; 
    }
    FastLED.show();
  }
}

BLYNK_WRITE(V6) {
  if(state && manual) {
    for(int i = 0; i <= NUM_LEDS; i++) {
      leds[i] = CRGB::Blue; 
    }
    FastLED.show();
  }
}

BLYNK_WRITE(V7) {
  if(state && manual) {
    for(int i = 0; i <= NUM_LEDS; i++) {
      leds[i] = CRGB::White; 
    }
    FastLED.show();
  }
}

BLYNK_WRITE(V9) {
  int preset = param.asInt();
  if(state && !manual && preset) {
    nextPattern();
  }
  FastLED.show();
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void colorFade() {
  for(int i = 0; i <= NUM_LEDS; i++) {
    leds[i].setHSV(gHue, 255, 255);
  }
}
