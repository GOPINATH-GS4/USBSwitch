#include "Keyboard.h"
#include "Mouse.h"


char mappings[10][50] = {
  "0", "1" , "2", "3", "4", "5", "6", "7" , "8", "9"
};
// JOYSTICK Initializations 

#define S_K A2 
#define S_Y A1 
#define S_X A0 
int sx = 0, sy = 0, sk = 0;
bool joyStickPressed = false;
bool mousePressed = false;
bool mouseEnabled = false;



// Mouse Initializtions 
int px = 0, py = 0, pk = 0;
int range = 12;               // output range of X or Y movement
int responseDelay = 2;       // response delay of the mouse, in ms
int threshold = range/4;      // resting threshold
int center = range/2;         // resting position value
int mouseReading[2];          // final mouse readings for {x, y}


// Switch Initializtions 

#define START_PIN 2
#define END_PIN 9
String inData;
int inChar = -1;
bool commandRecd = false;

void setup() { // initialize the buttons' inputs:
  for (int i = START_PIN; i <= END_PIN ; i++) {
      pinMode(i, OUTPUT);
      digitalWrite(i, LOW);
  }
  for (int i = START_PIN; i <= END_PIN ; i++) {
      pinMode(i, INPUT_PULLUP);
  }
  // Initialize analog pins for JoyStick 
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

 
  Serial.begin(9600);
  // initialize mouse & keybord control:
  Mouse.begin();
  Keyboard.begin();
 
}

void loop() {

  readCommand();

  if(commandRecd) {
    parseAndUnderstandCommand();
  }

  readJoyStick();

  if(joyStickPressed && mouseEnabled) {
   
    int x = getReading(sx);
    int y = -getReading(sy);
    
    Mouse.move(x,y,0);
    
    if (mousePressed) {
      if (!Mouse.isPressed(MOUSE_LEFT)) {
        Mouse.press(MOUSE_LEFT);
      }
    } else {
      if (Mouse.isPressed(MOUSE_LEFT)) {
         Mouse.release(MOUSE_LEFT);
      }
    }
     
  }
  int x  = LOW;
  for (int i = START_PIN; i <= END_PIN; i++) {
    
    if ((x = digitalRead(i)) == LOW) {  
      writeKeyBoard(mappings[i-2]);
    }
  } 
  delay(responseDelay);
}

int getReading(int value) {
  // Logic need to be refined, currenly just demostrating the movement 
  // read the analog input:
 

  // map the reading from the analog input range to the output range:
  int reading = map(value, 0, 1023, 0, range);

  // if the output reading is outside from the
  // rest position threshold,  use it:
  int distance = reading - center;

  if (abs(distance) < threshold) {
    distance = 0;
  }

  // return the distance for this axis:
  return distance;

}

void parseAndUnderstandCommand() {
  int index = -1;
  String cmd;
  if ((index = inData.indexOf(':')) != -1) {
    cmd = inData.substring(0, index);

    if(cmd.equals("M")) {
      int data = inData.substring(index+1).toInt();
      mouseEnabled = (data == 1);
    } else if (cmd.equals("P")) {
      String data = inData.substring(index+1);
      int idx = data.indexOf(':');
      int pin = data.substring(0, idx).toInt();
      String value = data.substring(idx+1).c_str();
      if (pin >= START_PIN && pin <= END_PIN) {
        strcpy(mappings[pin-2], value.c_str());
      }
    }
  }
}
  
void writeKeyBoard(char *value) {

  for(int i = 0; i < strlen(value); i++) 
    Keyboard.write(*(value + i));
}

void readJoyStick() {
  sk = analogRead(S_K);
  sy = analogRead(S_Y);
  sx = analogRead(S_X);

  if(sx != px || sy != py || sk != pk) 
    joyStickPressed = true;
  px = sx;
  py = sy;
  pk = sk;
  mousePressed = (sk == 0);
}
void readCommand() {
  
    byte numBytesAvailable= Serial.available();

    // if there is something to read
    inData = "";
    if (numBytesAvailable > 0){
        Serial.print("No of bytes:");
        Serial.println(numBytesAvailable);
        // store everything into "inData"
        int i = 0;
        for (i=0;i<numBytesAvailable;i++){
            inChar= Serial.read();
            //Serial.println((char) inChar);
            inData += (char) inChar;
        }
        Serial.print("Command Received: ");
        Serial.println(inData);
        commandRecd = true;
    }
}



