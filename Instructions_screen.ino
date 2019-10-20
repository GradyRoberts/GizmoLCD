#include <Adafruit_GFX.h>
//#include <Adafruit_TFTLCD.h>
#include <MCUFRIEND_kbv.h>

MCUFRIEND_kbv tft;

int joyPicPos = 0;
int joyPicDir = 1;//-1,1
int x1,y1,x2,y2,x3,y3,x4,y4;

void setup() 
{
  Serial.begin(115200);
  tft.reset();
  tft.begin(tft.readID());
  tft.fillScreen(0x0000);
}

void loop() 
{
  delay(500);
  drawInstructionsScreen(0);
}

void drawInstructionsScreen(int game) //0=TestGame  //TODO: add game picture logic
{
  String gameName;
  String dirText;
  String buttonsText;

  tft.fillTriangle(x1,y1,x3,y3,x4,y4,0x0000);
  tft.fillTriangle(x1,y1,x2,y2,x3,y3,0x0000);
  tft.fillCircle((x3+x4)/2,(y3+y4)/2,20,0x0000);
  
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
  
  //tft.fillScreen(0x0000);

  //picture Placeholder
  tft.fillRect(110,8,100,100,0xFFFF);

  //Game Name Text
  tft.setTextSize(4);
  tft.setTextColor(0xFFFF);
  //20*numchar+4*(numvhar-1)
  int numChar = gameName.length();
  int twidth = 20*numChar+4*(numChar-1);
  tft.setCursor((320-twidth)/2,120);
  tft.print(gameName);

  //Joystick Picture Arrows
  #define joyPosX 100
  #define joyPosY 275
  tft.fillTriangle(joyPosX-25,joyPosY+40,joyPosX+25,joyPosY+40,joyPosX,joyPosY+70, 0x001F);//Top
  tft.fillTriangle(joyPosX-25,joyPosY-40,joyPosX+25,joyPosY-40,joyPosX,joyPosY-70, 0x001F);//Bot
  tft.fillTriangle(joyPosX-40,joyPosY-25,joyPosX-40,joyPosY+25,joyPosX-70,joyPosY, 0x001F);//Left
  tft.fillTriangle(joyPosX+40,joyPosY-25,joyPosX+40,joyPosY+25,joyPosX+70,joyPosY, 0x001F);//Right
  
  //Joystick Picture Stick
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

  tft.fillTriangle(x1,y1,x3,y3,x4,y4,0xB596);
  tft.fillTriangle(x1,y1,x2,y2,x3,y3,0xB596);
  tft.fillCircle((x3+x4)/2,(y3+y4)/2,20,0xF800);

  //Buttons
  #define butPosX1 210
  #define butPosY1 240
  #define butPosX2 270
  #define butPosY2 300
  #define butRad 20
  tft.fillCircle(butPosX1,butPosY1,butRad,0xF800);
  tft.fillCircle(butPosX2,butPosY2,butRad,0xF800);
  tft.setCursor(butPosX1 - 10, butPosY1 - 10);
  tft.print("A");
  tft.setCursor(butPosX2 - 10, butPosY2 - 10);
  tft.print("B");
}
