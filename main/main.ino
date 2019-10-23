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
// LCD
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv tft;
int w, h;
uint16_t scrollbuf[320];
char currentPage; // H -> Home, G -> Game select, I -> Instructions, O -> Game over

// Joystick animation
#define joyPosX 100
#define joyPosY 275
int joyPicPos = 0;
int joyPicDir = 1;//-1,1
int x1,y1,x2,y2,x3,y3,x4,y4;
unsigned long animTimer;



void drawRectButton(int x, int y, int w, int h, int textSize, char text[]) {
  tft.fillRect(x, y, w, h, RED);
  tft.drawRect(x, y, w, h, YELLOW);
  tft.setCursor(x, y);
  tft.setTextColor(WHITE); tft.setTextSize(textSize);
  tft.println(text);
}

void drawHomeScreen() {  
  tft.fillScreen(BLACK);
  
  //TOUCH TO START "Button"
  drawRectButton(10, 420, 300, 50, 4, "TAP TO START");

  drawLeaderboards();
}

void drawLeaderboards() {
  //Displaying the name of Game
  tft.setCursor(0, 10);
  tft.setTextColor(WHITE); tft.setTextSize(4);
  tft.print("GameName");

  //"HIGH SCORES"
  tft.setCursor(0, 50);e
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

void drawInstructionsScreen(int game) //0=TestGame  //TODO: add game picture logic
{
  String gameName;
  String dirText;
  String buttonsText;
  
  switch(game)
  {
    case 0:
      gameName = "TestGame";
      dirText = "Use the joystick to do stuff";
      buttonsText = "Use the buttons to do other stuff"; //Maybe have numbers over buttons image and have directions be loist of what buttons do
      break;
    default:
      dirText = "ERROR: GAME DOES NOT EXIST";
      buttonsText = "ERROR: GAME DOES NOT EXIST";
  }

  tft.fillScreen(BLACK);
  
  //picture Placeholder
  tft.fillRect(110, 8, 100, 100, WHITE);

  //Game Name Text
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  //20*numchar+4*(numvhar-1)
  int numChar = gameName.length();
  int twidth = 20*numChar+4*(numChar-1);
  tft.setCursor((320-twidth)/2, 120);
  tft.print(gameName);

  // Arrows
  tft.fillTriangle(joyPosX-25, joyPosY+40, joyPosX+25, joyPosY+40, joyPosX, joyPosY+70, 0x001F);//Bot
  tft.fillTriangle(joyPosX-25, joyPosY-40, joyPosX+25, joyPosY-40, joyPosX, joyPosY-70, 0x001F);//Top
  tft.fillTriangle(joyPosX-40, joyPosY-25, joyPosX-40, joyPosY+25, joyPosX-70, joyPosY, 0x001F);//Left
  tft.fillTriangle(joyPosX+40, joyPosY-25, joyPosX+40, joyPosY+25, joyPosX+70, joyPosY, 0x001F);//Right
  
  //Joystick Stick
  x1 = joyPosX-5;
  x2 = joyPosX+5;
  y1 = joyPosY;
  y2 = joyPosY;
  
  switch(joyPicPos)
  {
    case -1:
      joyPicPos = 0;
      x3 = joyPosX - 55;
      x4 = joyPosX - 65;
      y3 = joyPosY - 60;
      y4 = joyPosY - 60;
      break;
    case 0:
      joyPicPos += joyPicDir;
      joyPicDir *= -1;
      x3 = joyPosX + 5;
      x4 = joyPosX - 5;
      y3 = joyPosY - 90;
      y4 = joyPosY - 90;
      break;
    case 1:
      joyPicPos = 0;
      x3 = joyPosX + 65;
      x4 = joyPosX + 55;
      y3 = joyPosY - 60;
      y4 = joyPosY - 60;
      break;
    default:
      Serial.println("ERROR: Joystick picture position does not exist: " + joyPicPos);
  }

  tft.fillTriangle(x1, y1, x3, y3, x4, y4, 0xB596);
  tft.fillTriangle(x1, y1, x2, y2, x3, y3, 0xB596);
  tft.fillCircle((x3+x4)/2, (y3+y4)/2, 20, RED);

  //Buttons
  #define butPosX1 210
  #define butPosY1 240
  #define butPosX2 270
  #define butPosY2 300
  #define butRad 20
  tft.fillCircle(butPosX1, butPosY1, butRad, RED);
  tft.fillCircle(butPosX2, butPosY2, butRad, RED);
  tft.setCursor(butPosX1 - 10, butPosY1 - 15);
  tft.print("A");
  tft.setCursor(butPosX2 - 10, butPosY2 - 15);
  tft.print("B");
}

void updateInstructionsScreen() {
  // Clear stick
  tft.fillTriangle(x1, y1, x3, y3, x4, y4, BLACK);
  tft.fillTriangle(x1, y1, x2, y2, x3, y3, BLACK);
  tft.fillCircle((x3+x4)/2, (y3+y4)/2, 20, BLACK);

  // Redraw top arrow
  tft.fillTriangle(joyPosX-25, joyPosY-40, joyPosX+25, joyPosY-40, joyPosX, joyPosY-70, 0x001F);//Top
  
  // Move stick
  x1 = joyPosX-5;
  x2 = joyPosX+5;
  y1 = joyPosY;
  y2 = joyPosY;
  
  switch(joyPicPos)
  {
    case -1:
      joyPicPos = 0;
      x3 = joyPosX - 55;
      x4 = joyPosX - 65;
      y3 = joyPosY - 60;
      y4 = joyPosY - 60;
      break;
    case 0:
      joyPicPos += joyPicDir;
      joyPicDir *= -1;
      x3 = joyPosX + 5;
      x4 = joyPosX - 5;
      y3 = joyPosY - 90;
      y4 = joyPosY - 90;
      break;
    case 1:
      joyPicPos = 0;
      x3 = joyPosX + 65;
      x4 = joyPosX + 55;
      y3 = joyPosY - 60;
      y4 = joyPosY - 60;
      break;
    default:
      Serial.println("ERROR: Joystick picture position does not exist: " + joyPicPos);
  }

  // Draw stick
  tft.fillTriangle(x1, y1, x3, y3, x4, y4, 0xB596);
  tft.fillTriangle(x1, y1, x2, y2, x3, y3, 0xB596);
  tft.fillCircle((x3+x4)/2, (y3+y4)/2, 20, RED);
}

void drawGameOverScreen(int score) {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 10);
  tft.setTextColor(WHITE); tft.setTextSize(3);
  tft.println("Game Over...");
  tft.print("Score: "); tft.println(score);

  tft.setTextColor(BLACK); tft.setTextSize(5);
  
  // Left char
  tft.fillTriangle(w/2-25-60, h/2-35, w/2-25-10, h/2-35, w/2-25-35, h/2 - 78, BLUE); // Top triangle
  tft.fillRect(w/2-25-60, h/2-25, 50, 50, WHITE); // Box
  tft.setCursor(w/2-25-60+12, h/2-25+10); tft.print("A"); // Char
    
  // Middle char
  tft.fillTriangle(w/2-25, h/2-35, w/2-25+50, h/2-35, w/2, h/2 - 78, BLUE); // Top triangle
  tft.fillRect(w/2-25,h/2-25,50,50,WHITE); // Box
  tft.setCursor(w/2-25+12, h/2-25+10); tft.print("A"); // Char

  // Right char
  tft.fillTriangle(w/2-25+60, h/2-35, w/2-25+110, h/2-35, w/2-25+85, h/2 - 78, BLUE); // Top triangle
  tft.fillRect(w/2-25+60, h/2-25, 50, 50, WHITE); // Box
  tft.setCursor(w/2-25+60+12, h/2-25+10); tft.print("A"); // Char
  
  drawRectButton(10, 420, 300, 50, 4, "SELECT GAME");
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

TSPoint readTS() {
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

  if (p.z >= MINPRESSURE && p.z <= MAXPRESSURE) {
    Serial.print("("); Serial.print(p.x); Serial.print(", "); Serial.print(p.y); Serial.print(", "); Serial.print(p.z); Serial.println(")");
  }

  return p;
}


void setup() {
  Serial.begin(9600);

  pinMode(13, OUTPUT);
  
  uint16_t id = tft.readID();
  tft.reset();
  tft.begin(id);

  w = tft.width(); h = tft.height();

  Serial.println("Init...");
  Serial.print("w = "); Serial.println(w);
  Serial.print("h = "); Serial.println(h);
  Serial.println("Starting on home screen.\n");
  
  drawHomeScreen();
  currentPage = 'H';
}

void loop() {
  TSPoint p = readTS();

  // Home screen
  if (currentPage == 'H') {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      if (p.y >= 420) {
        //Serial.println("Switching to game select screen!\n");
        //currentPage = 'G';
        //drawGameSelectScreen(0);

        Serial.println("Switching to instruction screen!\n");
        currentPage = 'I';
        drawInstructionsScreen(0);
        animTimer = millis();
      }
    }
  }

  // Instructions screen
  //    Show this screen while game is playing and wait for serial from PROPELLER
  else if (currentPage == 'I') {
    if (millis() - animTimer >= 400) {
      Serial.println("Updating instructions screen");
      updateInstructionsScreen();
      animTimer = millis();
    }
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      Serial.println("Switching to game over screen!\n");
      currentPage = 'O';
      drawGameOverScreen(69);
    }
  }
  
  // Game over screen
  else if (currentPage == 'O') {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      if (p.y >= 420) {
        Serial.println("Switching to home screen!\n");
        currentPage = 'H';
        drawHomeScreen();
      }
    }
  }

}
