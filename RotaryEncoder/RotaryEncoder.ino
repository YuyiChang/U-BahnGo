/*
 * Display FWD or REV when rotating the rotary encoder
 * Pin connection
 *  CLK - SCL: Serial clock  pinA Red 3
 *   DA - SDA: Serial Data   pinB Orange  4
 *   SW - Press: Send a pluse on the rncoder is presses
 *   
 *  20 pulsed per circle
 *  Serial Band 9600
  */

// Digital pin definitions
enum enDigitalPins
{
  // Rotary encoder input lines
  encoderSCL=3,
  encoderSDA=4,
  dpInEncoderPress=5,
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
  digitalWrite(dpInEncoderPress, HIGH);
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
      }
      else {
        counter--;
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


void setup()
{
  // configure the pins
  ResetEncoder();

  // init serial communication
  Serial.begin(9600); 
  Serial.println("Ready to begin");
}


void loop()
{
  ReadEncoder();
}
