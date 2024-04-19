#include <SoftwareSerial.h>

int debounceDelay = 20;

struct Button {  //button object
  int buttonPin;
  int currButtonState;
  unsigned long lastDebounceTime;

  Button(int buttonPin_) {  //constructor
    buttonPin = buttonPin_;
   currButtonState;
   lastDebounceTime = 0;
  }

  int getReading() { return digitalRead(buttonPin); }  //return current button reading

void checkButton() { //implemnts button debouncing while also changing displayNumber on press
  int reading = getReading();

  if ((millis() - lastDebounceTime) > debounceDelay) {
  
    if (reading != currButtonState) {
      currButtonState = reading;
      
      if (currButtonState == LOW) {  //button pressed
         if (buttonPin == 9) { Serial.print("0"); } 
         if (buttonPin == 8) { Serial.print("1"); }
         if (buttonPin == 3) { Serial.print("2"); }
           if (buttonPin == 2) { Serial.print("3"); }
      }
      }
      lastDebounceTime = millis();
    }
  }
};

Button leftButton(9);
Button rightButton(8);
Button upButton(3);
Button downButton(2);

void setup() {
    Serial.begin(9600);

 pinMode(leftButton.buttonPin, INPUT_PULLUP);
  pinMode(rightButton.buttonPin, INPUT_PULLUP);
   pinMode(upButton.buttonPin, INPUT_PULLUP);
  pinMode(downButton.buttonPin, INPUT_PULLUP);
}

void loop() {
   leftButton.checkButton();
 rightButton.checkButton();
  upButton.checkButton();
 downButton.checkButton();
}
