#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h>
#endif

#define LED_PIN    4
#define CUBE_COUNT 1
#define LEDS_PR_CUBE_COUNT 6
const uint8_t LED_COUNT = CUBE_COUNT*LEDS_PR_CUBE_COUNT;


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

const Color BLACK = { 0x00, 0x00, 0x00 };
const Color WHITE = { 0xFF, 0xFF, 0xFF };
const Color RED = { 0xFF, 0, 0 };
const Color GREEN = { 0, 0xFF, 0 };
const Color BLUE = { 0, 0, 0xFF };
const Color PINK = { 0xFF, 0x14, 0x93 };
const Color ORANGE = { 0xFF, 0xA5, 0x00 };
const Color YELLOW = { 0xFF, 0xFF, 0x00 };
const Color MAGENTA = { 0xFF, 0x00, 0xFF };
const Color CYAN = { 0x00, 0xFF, 0xFF };

const Color ColorTable[] = {
  RED,
  GREEN,
  BLUE,
  PINK,
  ORANGE,
  YELLOW,
  MAGENTA,
  CYAN,
  WHITE
};
const uint8_t COLOR_TABLE_LEN = sizeof(ColorTable) / sizeof(Color);


void setLedColor(Color colors[], uint8_t ledIdx,  const Color* c) {
  Color* color = &colors[ledIdx];
  memcpy(color, c, sizeof(Color));
}

void setAllLedsColor(Color colors[], int len, const Color* c) {
  for(int i = 0; i < len; ++i) {
    setLedColor(colors, i, c);
  }
}

const uint8_t SIDE_LED_IDX[] = { 1, 2, 3, 5 };

void setSideColor(Color colors[], uint8_t cubeIdx, uint8_t sideIdx, const Color* color) {
    setLedColor(colors, cubeIdx*LEDS_PR_CUBE_COUNT + SIDE_LED_IDX[sideIdx], color);
}

void setCubeColor(Color colors[], uint8_t cubeIdx, const Color* color) {
  for (int i = 0; i < LEDS_PR_CUBE_COUNT; ++i) {
    setLedColor(colors, cubeIdx*LEDS_PR_CUBE_COUNT + i, color);
  }
}

void setStripColor(Color colors[], uint8_t ledCount) {
  for(int i = 0; i < ledCount; ++i) {
    Color* c = &colors[i];
    strip.setPixelColor(i, c->r, c->g, c->b);
  }
}

void colorFade(uint8_t numLeds, Color initialColors[], Color targetColors[], uint32_t stepDelayMillis) {
  Color currentColors[numLeds];
  memcpy(currentColors, initialColors, numLeds*sizeof(Color));
  bool transitionDone = false;

  while(!transitionDone) {
    transitionDone = true;
    for(uint8_t i = 0; i < numLeds; ++i) {

      Color* currentColor = &currentColors[i];
      Color* targetColor = &targetColors[i];

      if (memcmp(currentColor, targetColor, sizeof(Color)) != 0) {
        transitionDone = false;

        if (currentColor->r < targetColor->r) ++currentColor->r;
        if (currentColor->r > targetColor->r) --currentColor->r;

        if (currentColor->g < targetColor->g) ++currentColor->g;
        if (currentColor->g > targetColor->g) --currentColor->g;

        if (currentColor->b < targetColor->b) ++currentColor->b;
        if (currentColor->b > targetColor->b) --currentColor->b;

        strip.setPixelColor(i, currentColor->r, currentColor->g, currentColor->b);
      }

    }

    strip.show();
    delay(stepDelayMillis);
  }
}

void colorFadeCubes() {
  Color initialColors[LED_COUNT];
  Color targetColors[LED_COUNT];

  for (int i = 0; i < COLOR_TABLE_LEN*2; ++i) {
    
    for(int j = 0; j < CUBE_COUNT; ++j) {
        const Color* initialColor = &ColorTable[(i+j) % COLOR_TABLE_LEN];
        const Color* targetColor = &ColorTable[(i+j+1) % COLOR_TABLE_LEN];

        setCubeColor(initialColors, j, initialColor);
        setCubeColor(targetColors, j, targetColor);
    }
    colorFade(LED_COUNT, initialColors, targetColors, 10);
    delay(2000);
  }
}

const Color SpinColorTable[] {
  RED,
  GREEN,
  BLUE
};
const uint8_t SPIN_COLOR_TABLE_LEN = sizeof(SpinColorTable) / sizeof(Color);

void colorSpin() {
  
  Color ledColors[LED_COUNT];
  const Color* bgColor = &BLACK;

  for (int iterationCount = 0; iterationCount < 10; ++iterationCount) {
    for (int rotationCount = 0; rotationCount < 4; ++rotationCount) {
      for (int sideIdx = 0; sideIdx < 4; ++sideIdx) {
        for (int cubeIdx = 0; cubeIdx < CUBE_COUNT; ++cubeIdx) {
          
          const Color* fgColor = &SpinColorTable[(iterationCount+cubeIdx) % SPIN_COLOR_TABLE_LEN];
          
          setCubeColor(ledColors, cubeIdx, bgColor);
          setSideColor(ledColors, cubeIdx, sideIdx, fgColor);
        }

        setStripColor(ledColors, LED_COUNT);
        strip.show();
        delay(100);
      }
    }
  }
}

void lightSingleCube(Color ledColors[], uint8_t cubeIdx, const Color* bgColor, const Color* fgColor) {
  setAllLedsColor(ledColors, LED_COUNT, bgColor);
  setCubeColor(ledColors, cubeIdx, fgColor);
}

void cubeHop() {
  Color ledColors[LED_COUNT];
  const Color* bgColor = &BLACK;
  const uint16_t delayMillis = 400;

  for (int iterationCount = 0; iterationCount < 10; ++iterationCount) {
    const Color* fgColor = &ColorTable[iterationCount % COLOR_TABLE_LEN];

    for (int jumpCount = 0; jumpCount < 1; ++jumpCount) {
      for (int cubeIdx = 0; cubeIdx < CUBE_COUNT; ++cubeIdx) {
        lightSingleCube(ledColors, cubeIdx, bgColor, fgColor);
        setStripColor(ledColors, LED_COUNT); 
        strip.show();
        delay(delayMillis);
      }
      for (int cubeIdx = CUBE_COUNT-2; cubeIdx > 0; --cubeIdx) {
        lightSingleCube(ledColors, cubeIdx, bgColor, fgColor);
        setStripColor(ledColors, LED_COUNT); 
        strip.show();
        delay(delayMillis);
      }
    }
  }
}


void cubeSlide() {
  Color currentColors[LED_COUNT];
  Color targetColors[LED_COUNT];
  const Color* bgColor = &BLACK;
  setAllLedsColor(currentColors, LED_COUNT, bgColor);
  setStripColor(currentColors, LED_COUNT);
  strip.show();

  for (int iterationCount = 0; iterationCount < 10; ++iterationCount) {
    const Color* fgColor = &ColorTable[iterationCount % COLOR_TABLE_LEN];

    for (int jumpCount = 0; jumpCount < 1; ++jumpCount) {
      for (int cubeIdx = 0; cubeIdx < CUBE_COUNT; ++cubeIdx) {
        lightSingleCube(targetColors, cubeIdx, bgColor, fgColor);
        colorFade(LED_COUNT, currentColors, targetColors, 5);
        memcpy(currentColors, targetColors, sizeof(currentColors));
      }
      for (int cubeIdx = CUBE_COUNT-2; cubeIdx > 0; --cubeIdx) {
        lightSingleCube(targetColors, cubeIdx, bgColor, fgColor);
        colorFade(LED_COUNT, currentColors, targetColors, 5);
        memcpy(currentColors, targetColors, sizeof(currentColors));
      }
    }
  }
}




// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(uint16_t c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(uint16_t i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(uint16_t c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void setup() {
  strip.begin();
  strip.show(); 
  strip.setBrightness(50);
}

void loop() {
  // cubeHop();
  // cubeSlide();
  // colorSpin();
  // colorFadeCubes();

  // Fill along the length of the strip in various colors...
  colorWipe(strip.Color(255,   0,   0), 500); // Red
  colorWipe(strip.Color(  0, 255,   0), 500); // Green
  colorWipe(strip.Color(  0,   0, 255), 500); // Blue

  // Do a theater marquee effect in various colors...
  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
  theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness

  rainbow(10);             // Flowing rainbow cycle along the whole strip
  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
}

