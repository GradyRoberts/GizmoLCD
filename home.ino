#include <SPI.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Touchscreen
#define YP A2
#define XM A3
#define YM 8
#define XP 9

#define TS_MINX 0
#define TS_MAXX 319

#define TS_MINY 0
#define TS_MAXY 479




/* ---FUNCTIONS--- */
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef READGRAM
#define READGRAM(x, y, buf, w, h)  tft.readGRAM(x, y, buf, w, h)
#endif

void drawHomeScreen();
void drawSelectScreen();
void drawInstructionScreen(uint8_t game);
void drawGameOverScreen(uint8_t game);

void drawLeaderboards();


/* ---VARIABLES--- */
MCUFRIEND_kbv tft;
int x, y, w, h;
uint16_t scrollbuf[320];
char currentPage; // H -> Home, G -> Game select, I -> Instructions, O -> Game over


void drawHomeScreen() {
  tft.fillScreen(BLACK);
  tft.setCursor(0,25);
  tft.setTextColor(WHITE); tft.setTextSize(4);
  tft.println("Tap to start!");

  drawLeaderboards();
}

void drawLeaderboards() {
  tft.fillRect(0, 100, w, h-100, WHITE);
}

void setup() {
  Serial.begin(9600);

  tft.reset();
  tft.begin(tft.readID());

  w = tft.width(); h = tft.height();
  
  drawHomeScreen();
  currentPage = 'H';
}

void loop() {
  if (currentPage == 'H') {
    
  }

}
