#include "MPPTController.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include <ACS712.h>

/*
 * Technology Demonstration of a Maximum Power Point Transfer Controller using the "Peturb and Observe" Algorithm
 * 
 * Theory of Operation
 * 
 * We are measuring voltage and current at the battery, so we can also calculate power
 * 
 * if Power_now > Power_prev {
 *  if Volts_now > Volts_prev {
 *    increase PWM output
 *  }
 *  else {
 *    decrease PWM output
 *  }
 * else {
 *  if Volts_now > Volts_prev {
 *    decrease PWM output
 *  }
 *  else {
 *    increase PWM output
 *  }
 * }
 * 
 * PINS
 * D0  - TxD
 * D1  - RxD
 * D2  - Rotary Encoder
 * D3  - Rotart Encoder
 * D4  - Encoder Pushbutton
 * D5  - PWM Out to MOSFET
 * D6  -
 * D7  - Panel Cutoff Relay
 * D8  -
 * D9  -
 * D10 - (SS)
 * D11 - (MOSI)
 * D12 - (MISO)
 * D13 - (SCK)
 * A0  - Vout
 * A1  - Iout
 * A2  - 
 * A3  -
 * A4  - SDA
 * A5  - SCL
 * A6  -
 * A7  -
 */

LiquidCrystal_I2C lcd(0x3F, 16, 2);
ACS712 acs(IOUT, 5.0, 1023, 66);

float readVolts(void){
#ifdef _DEBUG
  Serial.print("readVolts: ");
  Serial.println(String(analogRead(VOUT)));
#endif  
  return (float)analogRead(VOUT) * (float)VOLTAGE_SCALE;
}

float readAmps(void) {
  int in = acs.mA_DC() / 1000;
#ifdef _DEBUG
  Serial.print("readAmps: ");
  Serial.println(String(in));
#endif  
  return (float)in;
}

int calculateDutyCycle(float value) {
  return int((float)value/(float)256 * 100);
}

float calculatePower(float vIn, float iIn) {
  float power = 0.0;
  power = (vIn * iIn);
  return power;
}

void pwmUp(void) {
  if(powerOut < 256)
    powerOut++;
}

void pwmDown(void) {
  if(powerOut > 0)
    powerOut--;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
#ifdef _DEBUG  
  Serial.print("MPPT Controller ");
  Serial.println(VERSION);
#endif
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Ver:");
  lcd.setCursor(0,4);
  lcd.print(VERSION);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_BUTTON, INPUT_PULLUP);
  pinMode(RELAY, OUTPUT);
  pinMode(IOUT, INPUT);
  pinMode(VOUT, INPUT);

  // Turn on the solar panel
  digitalWrite(RELAY, HIGH);
  analogWrite(DRIVE, 0);
}

void loop() {
  lastPower = currentPower;
    lastVoltage = volts;

  volts = readVolts();
  amps = readAmps();
  currentPower = calculatePower(volts, amps);
#ifdef _DEBUG  
/*  Serial.print("Vin, Iin, Power, PWM: ");
  Serial.print(String(volts));
  Serial.print(", ");
  Serial.print(String(amps));
  Serial.print(", ");
  Serial.print(String(currentPower));
  Serial.print(", ");
  Serial.print(String(calculateDutyCycle((float)powerOut)));
  Serial.println(); */
#endif
//  lcd.clear();
  lcd.setCursor(0,0);
//lcd.print("----------------");
  lcd.print("Watts      PWM %");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print(String(currentPower));
  lcd.setCursor(13,1);
  lcd.print(String(calculateDutyCycle((float)powerOut)));
  analogWrite(DRIVE, powerOut);
  if(lastPower < currentPower) {
    if(volts > lastVoltage)
      pwmUp();
    else
      pwmDown();
  }
  if(lastPower > currentPower) {
    if(volts < lastVoltage)
      pwmUp();
    else
      pwmDown();
  }
  delay(500);
}
