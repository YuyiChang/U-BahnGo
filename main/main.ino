#define NDEBUG
#include <Keyboard.h>

const int LED = 13;
static int LED_STATUS = false;

const int INITIAL_BUTTON = 2;

const int PUSH_BUTTON[3] = { 2, 3, 4 };
bool PUSH_BUTTON_STATUS[3] = { false, false, false };
const int PUSH_BUTTON_KEY[3] = { KEY_F5, KEY_F6, KEY_F12 };

const int SWITCH[3] = { 5, 6, 7 };
bool SWITCH_STATUS[3] = { false, false, false };
const int SWITCH_KEY[3] = { KEY_F11, 'y', 'p'};

//const int SWITCH[3] = { 7 };
//bool SWITCH_STATUS[3] = { false};

const int SPEED_BUTTON[4] = { 8, 9, 10, 11 };
const int SPEED_BUTTON_KEY[4] = { '7', '4', KEY_F7, KEY_F11 };

void setup() {
  pinMode(A0, OUTPUT);
  pinMode(LED, OUTPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(PUSH_BUTTON[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 3; i++) {
    pinMode(SWITCH[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(SPEED_BUTTON[i], INPUT_PULLUP); 
  }
  ResetEncoder();
  
  LED_SWITCH();
  Serial.begin(9600);
  Serial.println("Waiting to start");

  while (!digitalRead(INITIAL_BUTTON)) 
    ;
  //digitalWrite(LED, LOW);
  LED_SWITCH();
  Serial.println("Ready");
  delay(2000);
  Keyboard.begin();
}

void loop() {
  analogWrite(A0, 1024);
//  Serial.println(digitalRead(2));
//  ReadEncoder();
  ReadPushButton();
  ReadSwitch();
  ReadSpeedButton();
}

void LED_SWITCH() {
  if (LED_STATUS) {
    digitalWrite(LED, LOW);
  } else {
    digitalWrite(LED, HIGH);
  }
  LED_STATUS = !LED_STATUS;
}

void ReadSpeedButton() {
  for (int i = 0; i < 4; i++) {
    if (!digitalRead(SPEED_BUTTON[i])) {
      Serial.println("Speed " + i);
      Keyboard.press(SPEED_BUTTON_KEY[i]);
      Keyboard.releaseAll();
    }
  }
}

void ReadPushButton() {
  bool newStatus[3] = { false, false, false };
  for (int i = 0; i < 3; i++) {
    newStatus[i] = digitalRead(PUSH_BUTTON[i]);
    if (newStatus[i] != PUSH_BUTTON_STATUS[i]) {
      Serial.print("PushButton ");
      Serial.print(PUSH_BUTTON[i]);
      Serial.print(" changed to ");
      Serial.println(newStatus[i]);

      Keyboard.press(PUSH_BUTTON_KEY[i]);
      Keyboard.releaseAll();
      LED_SWITCH();
      //PUSH_BUTTON_STATUS[i] = newStatus[i];

      delay(100);
    }
  }

}

void ReadSwitch() {
  bool newStatus[3] = { false, false, false };
  for (int i = 0; i < 3; i++) {
    newStatus[i] = digitalRead(SWITCH[i]);
    if (newStatus[i] != SWITCH_STATUS[i]) {
      Serial.print("Switch ");
      Serial.print(SWITCH[i]);
      Serial.println(" turned");
      SWITCH_STATUS[i] = newStatus[i];

      Keyboard.press(SWITCH_KEY[i]);
      Keyboard.releaseAll();
    }
  }
}


// Digital pin definitions
enum enDigitalPins
{
  // Rotary encoder input lines
  encoderSCL=A1,
  encoderSDA=A2,
  dpInEncoderPress=0,
};

static void ResetEncoder()
{
  // Rotary encoder input lines
  // Configure as input, turn on pullup resistors
  pinMode(encoderSCL, INPUT);
  digitalWrite(encoderSCL, HIGH);
  pinMode(encoderSDA, INPUT);
  digitalWrite(encoderSDA, HIGH);
  pinMode(dpInEncoderPress, INPUT);
}

void _lowlevel_ReadEncoder(int &rotate, int& press)
{
  rotate = (digitalRead(encoderSDA) * 2) + digitalRead(SCL);
  press = digitalRead(dpInEncoderPress);
}

void ReadEncoder()
{
  int Position, Press;
  int isForward = 0;
  int counter = 0;
  int currCount = 0;
  
  ResetEncoder();
  Serial.println("Reading the encoder... press a key to abort.");
  _lowlevel_ReadEncoder(Position, Press);
  while (!Serial.available())
  {
    int Position2, Press2;
    do
    {
      _lowlevel_ReadEncoder(Position2, Press2);
    } while ((Position2 == Position) && (Press2 == Press));
    if (Position2 != Position)
    {
      // "Forward" is shown by the position going from (0 to 1) or (1 to 3)
      // or (3 to 2) or (2 to 0).  Anything else indicates that the user is
      // turning the device the other way.  Remember: this is Gray code, not
      // binary.
      int isFwd = ((Position == 0) && (Position2 == 1)) ||
                  ((Position == 1) && (Position2 == 3)) ||
                  ((Position == 3) && (Position2 == 2)) ||
                  ((Position == 2) && (Position2 == 0));
      if (isFwd) {
        counter++;
        if (currCount - counter == 2) {
          Keyboard.press(KEY_UP_ARROW);
          Keyboard.releaseAll();
          currCount = counter;
          Serial.println("Forward");
        }
      }
      else {
        counter--;
        if (counter - currCount == 2) {
          Keyboard.press(KEY_DOWN_ARROW); 
          Keyboard.releaseAll();
          currCount = counter;
          Serial.println("Backward");
        }
      }
      Serial.print(isFwd ? "FWD " : "BWD ");
      Serial.println(counter);
    }
    if (Press2 != Press)
    {
      Serial.println(Press ? "Press" : "Release");
      counter = 0;
    }
    Position = Position2;
    Press = Press2;
  }
}

