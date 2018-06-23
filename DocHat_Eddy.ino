//#include <Mux.h>
//Mux mux(9, 8, 7, 6, A5);
////Mux mux(6, 7, 8, 9, A0);

#include <Servo.h>

#define JOY_BTN 12
#define JOY_Y A6
#define JOY_X A7

#define SCL 10
#define SDA 11

#include "SoftPWM.h"
#define LED_GREEN 3 // messed up wiring colors :P
#define LED_BLUE 4// messed up wiring colors :P
#define LED_RED 2 // messed up wiring colors :P

#define SERVO_PAN 5
#define SERVO_TILT 6
Servo srv_pan;  // create servo object to control a servo
Servo srv_tilt;  // create servo object to control a servo
uint16_t valX;
uint16_t valY;
#define MARGIN 10
#define TILT_MIN 0
#define TILT_MAX 180
#define ROT_MIN 0
#define ROT_MAX 180

const uint16_t AnalogPins[] = {A0, A1, A2, A3, A4, A5};

#define VCC 4.66 // Bad Voltage regulator

uint16_t I_Dark[] = {0, 0};
uint16_t I_Gain[] = {0, 0};
const uint8_t PhoDio_PINS[] = {A4, A5};
const uint8_t num_PhoDio = sizeof(PhoDio_PINS) / sizeof(PhoDio_PINS[0]);

void setup() {

  Serial.begin(9600);

  pinMode(OUTPUT, LED_GREEN);
  pinMode(OUTPUT, LED_BLUE);
  pinMode(OUTPUT, LED_RED);

  pinMode(OUTPUT, SERVO_PAN);
  pinMode(OUTPUT, SERVO_TILT);

  pinMode(INPUT, PhoDio_PINS[0]);
  pinMode(INPUT, PhoDio_PINS[1]);

  pinMode(INPUT_PULLUP, JOY_X);
  pinMode(INPUT_PULLUP, JOY_Y);

  pinMode(OUTPUT, 6);
  pinMode(OUTPUT, 7);
  pinMode(OUTPUT, 8);
  pinMode(OUTPUT, 9);

  srv_pan.attach(SERVO_PAN);  //
  srv_tilt.attach(SERVO_TILT);  //

  //  SoftPWMBegin();
  //  SoftPWMSet(LED_BLUEedpin, 0);
  //  SoftPWMSetFadeTime(LED_BLUE, 1000, 1000);
  //  SoftPWMSet(LED_BLUEedpin, 255);

  //analogReference(EXTERNAL)
  delay(500);
  memcpy(I_Dark, MeasureBrightness( 16 ), sizeof(I_Dark));
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  memcpy(I_Gain, MeasureBrightness( 16 ), sizeof(I_Gain));
  digitalWrite(LED_BUILTIN, LOW);
}

uint16_t *MeasureBrightness(uint16_t samples) { // put finger on it, or make it really dark :P
  //  uint8_t num_PhoDio = sizeof(I_Dark) / sizeof(I_Dark[0]);
  static uint16_t measurement[num_PhoDio];
  uint16_t tmp = 0;
  for (uint8_t jj = 0; jj < num_PhoDio; jj++) {
    for (uint8_t ii = 1; ii <= (samples + 1); ii++) {
      tmp = analogRead(PhoDio_PINS[jj]);
      if (ii == 1) {
        measurement[jj] = 0;
        continue;
      } else {
        measurement[jj] += tmp;
      }
    }
    measurement[jj] /= samples;

  }
  return measurement;
}

void Dark_Correction(uint16_t *vals) { // array is always passed by reference!
  //uint8_t NUM = sizeof(vals) / sizeof(vals[0]);
  uint16_t tmp = 0;
  for (uint8_t jj = 0; jj < num_PhoDio; jj++) {
    if (vals[jj] < I_Dark[jj]) {
      vals[jj] = 0;
    } else {
      vals[jj] -= I_Dark[jj];
    }
  }
}

void loop() {
  /**
     MUXER
  */
  //  for (int ii = 0; ii < 16; ii++) {
  //    Serial.print(mux.read(ii));
  //    Serial.print(" ");
  //  }
  //  Serial.println("");
  //  delay(50);


  /**
     Servo Reads
  */
  valX = analogRead(JOY_Y);            // reads the value of the potentiometer (value between 0 and 1023)
  valY = analogRead(JOY_X);            // reads the value of the potentiometer (value between 0 and 1023)
  valX = map(valX, 0, 1023, ROT_MIN + MARGIN, ROT_MAX - MARGIN);
  valY = map(valY, 0, 1023, TILT_MIN + MARGIN, TILT_MAX - MARGIN); // scale it to use it with the servo (value between 0 and 180)
  srv_pan.write(valX);                  // sets the servo position according to the scaled value
  srv_tilt.write(valY);
  delay(15);

  /**
     ANALOG READS OF PHOTODIODES
  **/
  uint16_t vals[num_PhoDio];
  memcpy(vals, MeasureBrightness( 4 ), sizeof(vals));
  bool Correct_Dark = true;
  if (Correct_Dark) {
    Dark_Correction(vals);
   // Dark_Correction(I_Gain);
  }

  bool Show_mV = false;
  Serial.print("Intensity: ");
  for (uint8_t ii = 0; ii < num_PhoDio; ii++) {
    Serial.print(Show_mV ? (uint16_t)(vals[ii] / 1023.0 * VCC * 1000) : vals[ii]);
    Serial.print(" ");
  }
  Serial.println("");
  Serial.print("Dark: ");
  for (uint8_t ii = 0; ii < num_PhoDio; ii++) {
    Serial.print(Show_mV ? (uint16_t)(I_Dark[ii] / 1023.0 * VCC * 1000) : I_Dark[ii]);
    Serial.print(" ");
  }
  Serial.println("");
  Serial.print("Gain: ");
  for (uint8_t ii = 0; ii < num_PhoDio; ii++) {
    Serial.print(Show_mV ? (uint16_t)(I_Gain[ii] / 1023.0 * VCC * 1000) : I_Gain[ii]);
    Serial.print(" ");
  }
  Serial.println("");

}
