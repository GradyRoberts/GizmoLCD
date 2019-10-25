#include <EEPROM.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

// Color definitions
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

// Scrolling
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

// Game select
String games[] = {"Tetris", "Snake", "Pong", "Space Invaders"};
int numGames = 4;
int currentGame = 1;
int triH = 50;
int triW = 70;
int triTapBuf = 5;
int triSep = 70; // pixels between the triangle and the screen middle
unsigned long selectTimer;

// Game over
char banned[][4] =                                                                                                                  {"fag","fgt","ngr","nig","cnt","dyk","knt","gay","gey","gei","gai","jew","joo","jap","wop","kik","kyk","poc","kyq","kkk","guc","guk","guq","azn"};
char nick[] = {'A', 'A', 'A'};
unsigned long txtTimer;

struct HighScore {
  char nick[3];
  unsigned int score;

  bool operator==(const HighScore &a) {
    return (strcmp(nick, a.nick) == 0) && (score==(a.score));
  }

  String toString() {
    return "NAME=" + String(nick) + "\nSCORE=" + String(score);
  }
};


int getTextWidth(String text, int textSize) {
  int w, numChrs;

  numChrs = text.length();
  w = textSize*(5*numChrs + numChrs - 1);
  
  return w;
}

int getTextHeight(String text, int textSize) {
  int h, numChrs;

  numChrs = text.length();
  h = textSize*7/2;

  return h;
}

void drawRectButton(int x, int y, int w, int h, int textSize, String text) {
  tft.fillRect(x, y, w, h, RED);
  tft.drawRect(x, y, w, h, WHITE);
  tft.drawRect(x-1, y-1, w+2, h+2, WHITE);

  int textW = getTextWidth(text, textSize);
  int textH = getTextHeight(text, textSize);
  while ( textW >= w || textH >= h ) {
    textW = getTextWidth(text, textSize-1);
    textH = getTextHeight(text, textSize-1);
    textSize--;
  }
  tft.setTextSize(textSize);
  tft.setCursor(x+w/2-textW/2, y+h/2-textH);
  
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

void redrawGameTitle(){
  drawRectButton(10, h/2-50, 300, 100, 4, games[currentGame]);
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

  redrawGameTitle();
  
  tft.fillTriangle(
    w/2-triW/2, h/2+triSep, 
    w/2, h/2+triSep+triH, 
    w/2+triW/2, h/2+triSep, 
    CYAN
  );

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

  tft.setTextColor(BLACK); tft.setTextSize(4);
  
  // Left char
  nick[0] = 'A';
  tft.fillTriangle(75, 205, 125, 205, 100, 162, BLUE); // Top triangle
  tft.fillRect(75, 215, 50, 50, WHITE); // Box
  tft.setCursor(90, 225); tft.print(nick[0]); // Char
  tft.fillTriangle(75, 275, 125, 275, 100, 318, BLUE); // Bottom triangle
    
  // Middle char
  nick[1] = 'A';
  tft.fillTriangle(135, 205, 185, 205, 160, 162, BLUE); // Top triangle
  tft.fillRect(135, 215, 50, 50, WHITE); // Box
  tft.setCursor(150, 225); tft.print(nick[1]); // Char
  tft.fillTriangle(135, 275, 185, 275, 160, 318, BLUE); // Bottom triangle

  // Right char
  nick[2] = 'A';
  tft.fillTriangle(195, 205, 245, 205, 220, 162, BLUE); // Top triangle
  tft.fillRect(195, 215, 50, 50, WHITE); // Box
  tft.setCursor(210, 225); tft.print(nick[2]); // Char
  tft.fillTriangle(195, 275, 245, 275, 220, 318, BLUE); // Bottom triangle
  
  drawRectButton(40, 420, 240, 50, 4, "SUBMIT");
}

void updateNick(TSPoint p) {
  bool upToDate = true;
  
  if ( pointIn(75, 162, 50, 50, p) ) { // Top Left
    if (nick[0] <= 'A') { nick[0] = 'Z'; }
    else { nick[0]--; }

    upToDate = false;
  }
  else if ( pointIn(135, 162, 50, 50, p) ) { // Top Middle
    if (nick[1] <= 'A') { nick[1] = 'Z'; }
    else { nick[1]--; }
    
    upToDate = false;
  }
  else if ( pointIn(195, 162, 50, 50, p) ) { // Top Right
    if (nick[2] <= 'A') { nick[2] = 'Z'; }
    else { nick[2]--; }

    upToDate = false;
  }

  else if ( pointIn(75, 275, 50, 50, p) ) { // Bottom Left
    if (nick[0] >= 'Z') { nick[0] = 'A'; }
    else { nick[0]++; }

    upToDate = false;
  }
  else if ( pointIn(135, 275, 50, 50, p) ) { // Bottom Middle
    if (nick[1] >= 'Z') { nick[1] = 'A'; }
    else { nick[1]++; }

    upToDate = false;
  }
  else if ( pointIn(195, 275, 50, 50, p) ) { // Bottom Right
    if (nick[2] >= 'Z') { nick[2] = 'A'; }
    else { nick[2]++; }

    upToDate = false;
  }

  if (!upToDate) { writeNick(); }
}

void writeNick() {
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  
  tft.fillRect(75, 215, 50, 50, WHITE); // Box
  tft.setCursor(90, 225); tft.print(nick[0]); // Char
  
  tft.fillRect(135, 215, 50, 50, WHITE);
  tft.setCursor(150, 225); tft.print(nick[1]);
  
  tft.fillRect(195, 215, 50, 50, WHITE);
  tft.setCursor(210, 225); tft.print(nick[2]);
}

bool checkIfBanned() {
  char tmp[4] = {nick[0]+32, nick[1]+32, nick[2]+32, '\0'};
  for (int i = 0; i < sizeof(banned)/sizeof(banned[0]); i++) {
    if (strcmp(tmp, banned[i]) == 0) {
      return true;
    }
  }
  return false;
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

bool pointIn(int x, int y, int w, int h, TSPoint p){
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    if(p.x > x && p.x < x+w){
      if(p.y > y && p.y < y+h){
        return true;
      }
    }
  }
  return false;
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
    if ( pointIn(0,420,w,h,p) ) {
      Serial.println("Switching to game select screen!\n");
      currentPage = 'G';
      drawGameSelectionScreen();
      selectTimer = millis();
    }
  }

  // Game select screen
  else if (currentPage == 'G') {
    if(pointIn(w/2-triW/2, h/2-triSep-triH, triW, triH, p)){
      if (millis() - selectTimer >= 100 || selectTimer > millis()) {
        Serial.println("Tapped on upper tri");
        currentGame--;
        if(currentGame < 0){
          currentGame = numGames-1;
        }
      
        redrawGameTitle();
        selectTimer = millis();
      }
    }
    else if(pointIn(w/2-triW/2, h/2+triSep, triW, triH, p)){
      if (millis() - selectTimer >= 100 || selectTimer > millis()) {
        Serial.println("Tapped on lower tri");
        currentGame++;
        if(currentGame >= numGames){
          currentGame = 0;
        }
        
        redrawGameTitle();
        selectTimer = millis();
      }
    }
    
    else if(pointIn(10, h/2-50, 300, 100, p)) {
      Serial.println("Switching to instruction screen!\n");
      currentPage = 'I';
      drawInstructionsScreen(0);
      animTimer = millis();
    }
  }

  // Instructions screen
  //    Show this screen while game is playing and wait for serial from PROPELLER
  else if (currentPage == 'I') {
    if (millis() - animTimer >= 400 || animTimer > millis()) {
      Serial.println("Updating instructions screen");
      updateInstructionsScreen();
      animTimer = millis();
    }
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      Serial.println("Switching to game over screen!\n");
      currentPage = 'O';
      drawGameOverScreen(69);
      txtTimer = millis();
    }
  }
  
  // Game over screen
  else if (currentPage == 'O') {
    if (millis() - txtTimer >= 100 || txtTimer > millis()) {
      updateNick(p);
      txtTimer = millis();
    }
    if ( pointIn(0,420,w,h,p) ) {
      if (checkIfBanned()) {
        nick[0] = 'N';
        nick[1] = 'A';
        nick[2] = 'H';
        writeNick();
      }
      Serial.println("Switching to home screen!\n");
      currentPage = 'H';
      drawHomeScreen();
    }
  }

}
