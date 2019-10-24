#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
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

#define TS_MINX 130
#define TS_MAXX 905

#define TS_MINY 75
#define TS_MAXY 930

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define MINPRESSURE 10
#define MAXPRESSURE 1000


/* ---FUNCTIONS--- */
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef READGRAM
#define READGRAM(x, y, buf, w, h)  tft.readGRAM(x, y, buf, w, h)
#endif

/* ---VARIABLES--- */
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv tft;
int x, y, w, h;
uint16_t scrollbuf[320];
//char currentPage; // H -> Home, G -> Game select, I -> Instructions, O -> Game over

String games[] = {"Tetris", "Snake", "Pong", "Space Invaders"};
int numGames = 4;
int currentGame = 1;

int triH = 50;
int triW = 70;
int triTapBuf = 5;
int triSep = 70; // pixels between the triangle and the screen middle


bool pointIn(int x, int y, int w, int h, int px, int py){
  if(px > x && px < x+w){
    if(py > y && py < y+h){
      return true;
    }
  }
  return false;
}

void redrawGameTitle(){
  tft.fillRect(0, h/2-triSep, w, triSep*2, BLACK); // cover the old title
  
  tft.setTextColor(GREEN); tft.setTextSize(4);
  tft.setCursor(40, h/2-15);
  tft.println(games[currentGame]); 
}

void drawGameSelectionScreen() {
  tft.fillScreen(BLACK);
  tft.setCursor(20,20);
  tft.setTextColor(RED); tft.setTextSize(4);
  tft.println("Pick a game");

  // https://thestempedia.com/tutorials/tft-graphics/
  tft.fillTriangle(
    w/2-triW/2, h/2-triSep, 
    w/2, h/2-triSep-triH, 
    w/2+triW/2, h/2-triSep, 
    CYAN
  );

  tft.fillTriangle(
    w/2-triW/2, h/2+triSep, 
    w/2, h/2+triSep+triH, 
    w/2+triW/2, h/2+triSep, 
    CYAN
  );

}

void setup() {
  Serial.begin(9600);

  pinMode(13, OUTPUT);
  
  uint16_t id = tft.readID();
  tft.reset();
  tft.begin(id);

  w = tft.width(); 
  h = tft.height();
  
  Serial.print("Init... ");
  Serial.println("Starting on home screen.");
  
  drawGameSelectionScreen();
  redrawGameTitle();
  //currentPage = 'H';
}

void loop() {
  // Get touch info
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  // scale from 0->1023 to tft.width
  p.x = p.x + p.y;   // p.x = 500    
  p.y = p.x - p.y;   // p.y = 500 - 300 = 200
  p.x = p.x - p.y;   // p.x = 500 - 200 = 300
  p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
  p.y = tft.height()-(map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));

  if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
    return;
  }
  
  Serial.print("Tap at ("); Serial.print(p.x);
  Serial.print(", "); Serial.print(p.y);
  Serial.println(")");

  if(pointIn(w/2-triW/2, h/2-triSep-triH, triW, triH, p.x, p.y)){
    Serial.println("Tapped on upper tri");
    currentGame--;
    if(currentGame < 0){
      currentGame = numGames-1;
    }
    
    redrawGameTitle();
  } else if(pointIn(w/2-triW/2, h/2+triSep, triW, triH, p.x, p.y)){
    Serial.println("Tapped on lower tri");
    currentGame++;
    if(currentGame >= numGames){
      currentGame = 0;
    }
    
    redrawGameTitle();
  }
  
//  if (p.x >= 30 && p.x <=130 && p.y >= 30 && p.y <= 90) {
//    Serial.println("Switching to game over screen!");
//    currentPage = 'O';
//    drawGameOverScreen(0); 
//  }

}
