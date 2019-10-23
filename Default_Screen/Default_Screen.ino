/*
  Default_Screen.ino
  320x480
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
//#include <LiquidCrystal.h>

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
//LiquidCrystal lcd(8,9,3,4,5,2);
int x, y, w, h;
uint16_t scrollbuf[320];
char currentPage; // H -> Home, G -> Game select, I -> Instructions, O -> Game over


void drawHomeScreen() {
  //TOUCH TO START "Button"
  tft.fillScreen(BLACK);
  tft.drawRect(0, 450, 319, 479, YELLOW);
  tft.fillRect(0, 450, 319, 479, RED);
  tft.setCursor(0, 470);
  tft.setTextColor(WHITE); tft.setTextSize(4);
  tft.println("TAP TO START!");

  drawLeaderboards();
}

void drawLeaderboards() {
  //Displaying the name of Game
  tft.setCursor(0, 25);
  tft.setTextColor(WHITE); tft.setTextSize(4);
  tft.print("GameName");

  //"HIGH SCORES"
  tft.setCursor(0, 50);
  tft.setTextColor(WHITE); tft.setTextSize(3);
  tft.print("HIGH SCORES");

  //"RANK"
  tft.setCursor(0, 75);
  tft.setTextColor(RED); tft.setTextSize(2);
  tft.print("RANK");

  //"SCORE"
  tft.setCursor(100, 75);
  tft.setTextColor(RED); tft.setTextSize(2);
  tft.print("SCORE");

  //"PLAYER"
  tft.setCursor(200, 75);
  tft.setTextColor(RED); tft.setTextSize(2);
  tft.print("PLAYER");

  //"1st, 2nd, 3rd"
  tft.setCursor(0, 100);
  tft.setTextColor(CYAN); tft.setTextSize(2);
  tft.print("1st");

  tft.setCursor(0, 200);
  tft.setTextColor(MAGENTA); tft.setTextSize(2);
  tft.print("2nd");

  tft.setCursor(0, 300);
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.print("3rd");

  //Scores
  tft.setCursor(100, 100);
  tft.setTextColor(CYAN); tft.setTextSize(2);
  tft.print("100"); //1st place score

  tft.setCursor(100, 200);
  tft.setTextColor(MAGENTA); tft.setTextSize(2);
  tft.print("99"); //2nd place score

  tft.setCursor(100, 300);
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.print("98"); //3rd place score

  //Player
  tft.setCursor(200, 100);
  tft.setTextColor(CYAN); tft.setTextSize(2);
  tft.print("ABC"); //1st place player

  tft.setCursor(200, 200);
  tft.setTextColor(MAGENTA); tft.setTextSize(2);
  tft.print("DEF"); //2nd place player

  tft.setCursor(200, 300);
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.print("GHI"); //3rd place player
}

void windowScroll(int16_t x, int16_t y, int16_t wid, int16_t ht, int16_t dx, int16_t dy, uint16_t *buf)
{
    if (dx) for (int16_t row = 0; row < ht; row++) {
            READGRAM(x, y + row, buf, wid, 1);
            tft.setAddrWindow(x, y + row, x + wid - 1, y + row);
            tft.pushColors(buf + dx, wid - dx, 1);
            tft.pushColors(buf + 0, dx, 0);
        }
    if (dy) for (int16_t col = 0; col < wid; col++) {
            READGRAM(x + col, y, buf, 1, ht);
            tft.setAddrWindow(x + col, y, x + col, y + ht - 1);
            tft.pushColors(buf + dy, ht - dy, 1);
            tft.pushColors(buf + 0, dy, 0);
        }
}

void setup() {
  Serial.begin(9600);
  
  pinMode(13, OUTPUT);
   
  uint16_t id = tft.readID();
  tft.reset();
  tft.begin(id);

  Serial.println("Init...");
  Serial.println("Starting on home screen.");
  
  w = tft.width(); h = tft.height();
  //lcd.begin(5, 3)

  drawHomeScreen();
  currentPage = 'H';
}

void loop() {
  /*
    if (currentPage == 'H') {
    drawHomeScreen();
    } else if (currentPage == 'G') {
    drawSelectScreen();
    } else if (currentPage == 'I') {
    continue;
    //drawInstructionsScreen();
    } else if (currentPage == 'O'){
    //drawGameOverScreen();
    } else {
    drawHomeScreen();
    }
  */
}
