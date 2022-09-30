 
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <ezButton.h>
#include <FastLED.h>

#define THROTTLE A0
#define YAW A1
#define PITCH A2
#define ROLL A3
#define AUX_PIN_1 5
#define ENCODER_1_SW 9
#define ENCODER_1_DT 2
#define ENCODER_1_CLK 4
#define ENCODER_2_SW 10
#define ENCODER_2_DT 3
#define ENCODER_2_CLK 6
#define LED_PIN A4
#define NUM_LEDS 4

#define CSN 7
#define CE 8

// The sizeof this struct should not exceed 32 bytes
// This gives us up to 32 8 bits channals
struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte AUX1;
  byte AUX2;
};

struct Trim {
  int throttle;
  int yaw;
  int pitch;
  int roll;
};

/*Create a unique pipe out. The receiver has to 
  wear the same unique code*/
const byte address[6] = "00001";

RF24 radio(CSN, CE); // CSN and CE pins
MyData data;
Trim trim;
Encoder leftTrim(ENCODER_1_DT, ENCODER_1_CLK);
Encoder rightTrim(ENCODER_2_DT, ENCODER_2_CLK);
ezButton leftTrimSwitch(ENCODER_1_SW);  // create ezButton object that attach to pin 0;
ezButton rightTrimSwitch(ENCODER_2_SW);  // create ezButton object that attach to pin 5;
CRGB leds[NUM_LEDS];

long lastLeftTrimPos = 0;
long lastRightTrimPos = 0;
bool settingYawTrim = true;
bool settingRollTrim = true;

void resetData() 
{
  //These are the start values of each channal
  // Throttle is 0 in order to stop the motors
  //127 is the middle value of the 10ADC.
    
  data.throttle = 0;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.AUX1 = 0;
  data.AUX2 = 0;
}

void saveTrim(){
  EEPROM.put(0, trim.throttle);
  EEPROM.put(2, trim.yaw);
  EEPROM.put(4, trim.pitch);
  EEPROM.put(6, trim.roll);
}

void setTrim(){
  trim.throttle = EEPROM.get(0, trim.throttle);
  trim.yaw = EEPROM.get(2, trim.yaw);
  trim.pitch = EEPROM.get(4, trim.pitch);
  trim.roll = EEPROM.get(6, trim.roll);
  // trim.throttle = 412;
  // trim.yaw = 490;
  // trim.pitch = 516;
  // trim.roll = 484;
}

void setup()
{
  Serial.begin(115200);
  pinMode(AUX_PIN_1, INPUT_PULLUP);
  // pinMode(AUX_PIN_2, INPUT_PULLUP);
  pinMode(ENCODER_1_SW, INPUT_PULLUP);
  pinMode(ENCODER_1_DT, INPUT_PULLUP);
  pinMode(ENCODER_1_CLK, INPUT_PULLUP);
  pinMode(ENCODER_2_SW, INPUT_PULLUP);
  pinMode(ENCODER_2_DT, INPUT_PULLUP);
  pinMode(ENCODER_2_CLK, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  leftTrimSwitch.setDebounceTime(50); // set debounce time to 50 milliseconds
  rightTrimSwitch.setDebounceTime(50); // set debounce time to 50 milliseconds

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed

  //Start everything up
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  resetData();
  setTrim();
}

/**************************************************/

// Returns a corrected value for a joystick position that takes into account
// the values of the outer extents and the middle of the joystick range.
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}

void loop()
{
  
  readTrim();  

  // The calibration numbers used here should be measured 
  // for your joysticks till they send the correct values.
  data.throttle = mapJoystickValues( analogRead(THROTTLE), 46, trim.throttle, 869, true);
  data.yaw      = mapJoystickValues( analogRead(YAW),  67, trim.yaw, 902, true );
  data.pitch    = mapJoystickValues( analogRead(PITCH), 103, trim.pitch, 945, true );
  data.roll     = mapJoystickValues( analogRead(ROLL), 53, trim.roll, 922, true );
  data.AUX1     = digitalRead(AUX_PIN_1); //The 2 toggle switches
  // data.AUX2     = digitalRead(AUX_PIN_2);

  radio.write(&data, sizeof(MyData));
  printEncoders();
  saveTrim();
}

void readTrim() {
  leftTrimSwitch.loop(); // MUST call the loop() function first
  rightTrimSwitch.loop(); // MUST call the loop() function first

  if(leftTrimSwitch.isPressed())
    settingYawTrim = !settingYawTrim; //Switch which axis is getting trim adjusted


  if(rightTrimSwitch.isPressed())
    settingRollTrim = !settingRollTrim; //Switch which axis is getting trim adjusted

  int leftTrimRead = leftTrim.read()/4;
  int rightTrimRead = rightTrim.read()/4;
  int leftTrimAdjust = 0;
  int rightTrimAdjust = 0;

  if(lastLeftTrimPos != leftTrimRead){
    leftTrimAdjust = (lastLeftTrimPos - leftTrimRead) * -1;
    lastLeftTrimPos = leftTrimRead;
  }
  
  if(lastRightTrimPos != rightTrimRead){
    rightTrimAdjust = (lastRightTrimPos - rightTrimRead) * -1;
    lastRightTrimPos = rightTrimRead;
  }  

  if(settingYawTrim){
    trim.yaw += leftTrimAdjust;
    leds[0] = CRGB::Blue;
    leds[1] = CRGB::Black;
  } else {
    leds[0] = CRGB::Black;
    leds[1] = CRGB::Blue;
    trim.throttle += leftTrimAdjust;
  }

  if(settingRollTrim){
    leds[2] = CRGB::Black;
    leds[3] = CRGB::Blue;
    trim.roll += rightTrimAdjust;
  } else {
    leds[2] = CRGB::Blue;
    leds[3] = CRGB::Black;
    trim.pitch += rightTrimAdjust;
  }

  FastLED.show();
}

void printEncoders(){
  Serial.print(settingYawTrim ? "Yaw: " : "Throttle: ");
  Serial.print(settingYawTrim ? trim.yaw : trim.throttle);
  Serial.print(", ");
  Serial.print(settingRollTrim ? "Roll: " : "Pitch: ");
  Serial.println(settingRollTrim ? trim.roll : trim.pitch);
}