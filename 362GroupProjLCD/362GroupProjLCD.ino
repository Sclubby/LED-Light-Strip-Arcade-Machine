#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

SoftwareSerial LEDConnection(8,9);
int gameMode = 0;
bool onMenu = true;

//lcd variables
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;  //pin locations for lcd
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
unsigned long timer = 30;
unsigned long lastTimePoint = 0;

//jump rope lcd var;
int playerWinning = 0;

void setInitialLCDSetup(int gameNumber){
 switch (gameNumber) {
  case 1: TugOfWarUpdateLCD(); gameMode = 1; break;
  case 2: colorPickerUpdateLCD(); gameMode = 2; break;
  case 3: jumpRopeUpdateLCD(); gameMode = 3; break;
 }
}

//////////////////////////// Jump Rope ///////////////////////////////////////////////////////////

void jumpRopeUpdateLCD(){
  lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Jump Rope");
}

void endJumpRope() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game End");
    lcd.setCursor(0, 1);
    lcd.print("Play Again?");
}

/////////////////////////// Color Picker //////////////////////////////////

void colorPickerUpdateLCD(){ //updates LCD at start of game
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Color Picker");
}

///////////////////// Tug of War ////////////////////////////

void endTugOfWar() { //Calculates who won the game and displays it on the lcd
   lcd.clear();
   lcd.setCursor(0, 0);
   if (playerWinning == 1) { lcd.print("Left wins"); }
  else if (playerWinning == 2) { lcd.print("Right Wins"); }
  else { lcd.print("Draw!"); }
  gameMode = 0;
}

void TugOfWarUpdateLCD(){ //Updates LCD (continously) during game
  lcd.clear();
  lcd.setCursor(0,0);
     lcd.print("Time Left: "); lcd.print(timer);
     lcd.setCursor(0, 1);
     switch (playerWinning) {
      case 0: lcd.print("FIGHT!!!"); break;
      case 1: lcd.print("Left Winning"); break; 
      case 2: lcd.print("Right Winning"); break;
     }
}

//////////////////////////////  Menu System ///////////////////////
int index = -1;

struct Game {
int gameNumber;
String gameTitle;
int screenOffset; //to make sure the title is centered
char infoChar;

Game(int num, String title, int offset, char info) { //used in menu system
  gameNumber = num;
  gameTitle = title;
   screenOffset = offset;
   infoChar = info;
}
};

Game tugOfWar = Game(1,"Tug of War",3,'T');
Game colorPicker = Game(2,"Color Picker",2,'C');
Game jumpRope = Game(3, "Jump Rope",3,'J');

Game games[3] =  {tugOfWar, colorPicker,jumpRope};

// 0 = left, 1 = right, 2 = up, 3 = down
void UpdateMenuScreen(char recData) {  //updates menu system based off of input
   switch (recData) {
     case '0': index--; break; //left
     case '1': index++; break; //right
     default:  //up or down
     if (index == -1) { return; }  //index is -1 at start if still -1 do nothing
     LEDConnection.print(games[index].infoChar);  //tell LEDStrip to play specific game
     onMenu = false;  //turn off menu
     setInitialLCDSetup(games[index].gameNumber);  //change LCD to that games set up
     return;
   }
   if (index < 0) { index = 2;} //loop index
   if (index == 3) { index = 0; }

   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("     Game "); lcd.print(games[index].gameNumber);
   lcd.setCursor(0, 1);
   lcd.print("< "); lcd.setCursor(games[index].screenOffset, 1); lcd.print(games[index].gameTitle);  lcd.setCursor(15, 1); lcd.print(">");

}

void setup() {
  pinMode(8,INPUT);
  pinMode(9,OUTPUT);
    LEDConnection.begin(9600);

   lcd.begin(16, 2);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("LEDSTRIP ARCADE");
      lcd.setCursor(0, 1);
     lcd.print("< SELECT GAME >");
}

void loop() {
 if (gameMode == 1 && millis() - lastTimePoint > 1000) {  //updates lcd in tug of war
   timer--;
   if (timer == 0) { LEDConnection.print("D"); endTugOfWar();}
  else { TugOfWarUpdateLCD(); }
  lastTimePoint = millis();
 }

   if (LEDConnection.available()) {  //for getting input from other arduinos (not in use right now)  
      char recData = LEDConnection.read();
         
         if (onMenu) {UpdateMenuScreen(recData); }

            if (recData == 'X') { playerWinning = 2; } //tug of war player 1 won
            if (recData == 'Y') { playerWinning = 1;  } //tug of war player 2 won
            if (recData == 'Z') { playerWinning = 0;  } //tug of war player draw
            if (recData == 'L') { endJumpRope(); } //jump rope end game
            if(recData == 'S') { jumpRopeUpdateLCD(); } //start new jump rope game
            if(recData == 'T') { endTugOfWar(); }
   }

}
