#include <FastLED.h>
#include <SoftwareSerial.h>
FASTLED_USING_NAMESPACE

//Overall system global variables
int gameMode = 0;  //0 = menu system, 1 = tug of war, 2 = color picker
unsigned long lastTimePoint = 0;
SoftwareSerial LCDConnection(6,7);

//ledstrip global variables
#define DATA_PIN    13 //data pin connection
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    30 //number of leds in strip
CRGB leds[NUM_LEDS];
#define BRIGHTNESS          80
#define FRAMES_PER_SECOND  120

//jump rope global variables
int cursor = 0;
int cursorSpeed = 50; //in milliseconds
int zoneStart = 12;
int zoneSize = 8;
bool jumped = false;
int totalJumps = 0;

//tug of war global variables
int player1Presses = 0;
int player2Presses = 0;
int contestedLED = 14;

//color picker global variables 
int selectedLED = 14;
int colorWheelIndex = 0;
CRGB savedColor[30];
CRGB colorWheel[] = {CRGB(250,0,250),CRGB(255,255,255),CRGB(255,0,0),CRGB(0,0,255),CRGB(0,255,0)};


//////  OVERALL LED FUNCTIONS  ///////////////////////////////////////////

void fillfromXtoY(int x, int y, CRGB color) {  //fills LEDS with given color from X to Y
  while (x <= y) {
    leds[x] = color;
    x++;
  }
}

void initialLEDSetUp(){ //Initial LED positioning for each mode
  switch(gameMode) {
    case 0:   fill_solid(leds,NUM_LEDS,CRGB::White); break;
    case 1:   fillfromXtoY(0,14,CRGB(255,0,0));  leds[14] = CRGB::White;  fillfromXtoY(15,29,CRGB(0,0,255));  break;
    case 2: fill_solid(leds,NUM_LEDS,CRGB::Black);  leds[selectedLED] = CRGB(250,0,250); break;
    case 3: fill_solid(leds,NUM_LEDS,CRGB::Black);  leds[0] = CRGB(255,0,0);  fillfromXtoY(zoneStart,zoneStart+zoneSize,CRGB(255,255,255)); break;
  }
  FastLED.show();
}

////////   TUG Of WAR FUNCTIONS //////////////////////////////////////////////////////

void detWinning(){
   if (contestedLED >= 15) { LCDConnection.print("X"); }
  else if (contestedLED <= 13) { LCDConnection.print("Y"); }
  else { LCDConnection.print("Z"); }
}

void tugOfWarUpdateLED(bool player1Won) {  //moves contested LED if one player reaches a given amount of presses
  if (!player1Won) {   leds[contestedLED] = CRGB::Red; contestedLED++; }
  else { leds[contestedLED] = CRGB::Blue; contestedLED--;}
     leds[contestedLED] = CRGB::White;
    FastLED.show();
    player1Presses = 0; player2Presses = 0;
    detWinning();
}

void tugOfWarLogic() { //runs main logic of game
   if (player1Presses >= 3) { tugOfWarUpdateLED(true);   player1Presses = 0; player2Presses = 0; }  //first player to 3 presses wins an LED, this continues until game ends
   if (player2Presses >= 3) { tugOfWarUpdateLED(false);   player1Presses = 0; player2Presses = 0;}
   if (contestedLED == 0 || contestedLED == 29) { gameMode = -1; LCDConnection.print("T"); }
   }

void tugOfWarButtons(char button){ //button logic for tug of war game, only left and right buttons are used and each just increments a counter
   if(button == '0') { 
            player1Presses++;
          } else if (button == '1') { 
             player2Presses++;
            }
            tugOfWarLogic();
  }

////////////////////////////  COLOR PICKER FUNCTIONS ////////////////

void colorPickerUpdateLED(int butPressed) {  //updates LED's every time a button is pressed
   if (butPressed == 1 || butPressed == 2) { //logic when moving to a new led
        if (colorWheelIndex == 0 && savedColor[selectedLED] == NULL) {fillfromXtoY(selectedLED,selectedLED,CRGB(0,0,0)); } //if led has no saved color and was just highlighted keep it off
    else { fillfromXtoY(selectedLED,selectedLED,savedColor[selectedLED]);  } //else keep the saved color
        colorWheelIndex = 0;
   }
    if (butPressed == 1) { selectedLED--; }  if (butPressed == 2) { selectedLED++; } //move selectedLED up or down depending on what button pressed

       if (selectedLED > 29) { selectedLED = 0; } if (selectedLED < 0) { selectedLED = 29; }  //loop values from both array and selected LED
       if (colorWheelIndex > 4) { colorWheelIndex = 0; } if (colorWheelIndex < 0) { colorWheelIndex = 4; }

  fillfromXtoY(selectedLED,selectedLED,colorWheel[colorWheelIndex]);  //set color of led based off colorWheelArray
  if (colorWheelIndex != 0) {savedColor[selectedLED] = colorWheel[colorWheelIndex]; } //save color to SavedColor array unless its the highlighter
   FastLED.show();
}

void colorPickerButtons(char button){ //button logic for color picker game
int butPressed = 0;
       switch (button) {
      case '0':  butPressed = 1;  break;  //left
      case '1':   butPressed = 2; break; //right
      case '2':  colorWheelIndex++; break; //up
      case '3': colorWheelIndex--;  break; //down
    }
    colorPickerUpdateLED(butPressed);
}

////////////////////////////////////// Jump Rope //////////////////

void jumpRopeGameEnd(){
  gameMode = -2;
  LCDConnection.print('L');
}

void decreaseZoneSize(){
  if (zoneSize > 2) { //decrease zone size
  zoneSize -= 2;  zoneStart++;
  fill_solid(leds,NUM_LEDS,CRGB::Black);  
  fillfromXtoY(zoneStart,zoneStart+zoneSize,CRGB(255,255,255));

  } 
  FastLED.show();
}

void jumpRopeUpdateLED(){
 if (totalJumps == 6) { decreaseZoneSize(); totalJumps = 0;}

  if (cursor < zoneStart || cursor > zoneStart + zoneSize) { leds[cursor] = CRGB::Black; }
  else { leds[cursor] = CRGB::White; }

  cursor++;

  if (cursor == 30) {  //reset cursor if jump was successful if not end game
    if (!jumped) { jumpRopeGameEnd(); return;}
    cursor = 0;   jumped = false; totalJumps++;  cursorSpeed--;
    }

  leds[cursor] = CRGB(255,0,0);
    FastLED.show();
}

void jumpRopeButtons(){
   int jumpPos = cursor;
   jumped = true;
   if (jumpPos < zoneStart || jumpPos > zoneStart + zoneSize) {  jumpRopeGameEnd(); }
}

void resetJumpRopeGame(){
   cursor = 0;  cursorSpeed = 50;  zoneStart = 12;
 zoneSize = 8;  jumped = false;  totalJumps = 0;
gameMode = 3;
LCDConnection.print("S");
initialLEDSetUp();
}
////////////////////////////////////////////////////////////

void setup() {
    Serial.begin(9600);
    pinMode(6,INPUT);
    pinMode(7,OUTPUT);
    LCDConnection.begin(9600);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  initialLEDSetUp();
}
  
  } 
   if (LCDConnection.available()) {  //for getting input from LCD
      char recData = LCDConnection.read();
      switch (recData)  {
         case 'T': gameMode = 1; initialLEDSetUp(); break; //set up for Tug of War
         case 'C': gameMode = 2; initialLEDSetUp(); break; //set up for color picker
         case 'J': gameMode = 3; initialLEDSetUp(); break; //set up for jump rope
         case 'D':  gameMode = -1; break; //end tug of war (on timer ending)
      }
   }
}


