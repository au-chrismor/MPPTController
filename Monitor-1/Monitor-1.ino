#include <math.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ACS712.h>

#define _DEBUG  1
#define VREF    5
#define VBATT   A0
#define IBATT   A1
#define BATTR1  1000000
#define BATTR2  91000
#define RELAY1  3
#define VOLT_MIN  10
#define VOLT_MAX  14

// 0x3F is another common I2C address.  If your LCD doesn't work,
// it may well be at an alternative address.
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display
// 30A = 66mV/A
// 20A = 100mV/A
// 5A  = 185mV/A
ACS712 acs(A1, (float)VREF, 1023, 185);

float lastVolts = 0.0;
float lastAmps = 0.0;
float lastWatts = 0.0;
float volts = 0.0;
float amps = 0.0;
float watts = 0.0;

float readVolts(void) {
  float scaleFactor = (float)BATTR2 / ((float)BATTR1 + (float)BATTR2);
  int in = analogRead(VBATT);
#ifdef _DEBUG
  Serial.print("readVolts: ");
  Serial.println(String(in));
#endif
  return ((float)in / 1024 * VREF) / scaleFactor;
}

float readAmps() {
  return abs((float)acs.mA_DC() / 1000);
}

float readPower(float v, float i) {
  return v * i;
}

void outputRelay(float v) {
  if(v >(float)VOLT_MIN && volts < (float)VOLT_MAX) {
    lcd.setCursor(0, 3);
    lcd.write('L');
    digitalWrite(RELAY1, HIGH);
  }
  else {
    lcd.setCursor(0, 3);
    lcd.write(' ');
    digitalWrite(RELAY1, LOW);
  }
}

/*
 * This display shows Voltage, Current and Power
 */
void screen01(float v, float i, float p) {
  lcd.setCursor(0, 0);
  lcd.print("Volts:");
  lcd.setCursor(7, 0);
  lcd.print("             ");
  lcd.setCursor(7, 0);
  lcd.print(String(v));
  lcd.setCursor(19, 0);
  lcd.setCursor(0, 1);
  lcd.print("Amps:");  
  lcd.setCursor(7, 1);
  lcd.print("             ");
  lcd.setCursor(7, 1);
  lcd.print(String(i));
  lcd.setCursor(19, 1);
  lcd.setCursor(0, 2);
  lcd.print("Watts:");  
  lcd.setCursor(7, 2);
  lcd.print("             ");
  lcd.setCursor(7, 2);
  lcd.print(String(p));
  lcd.setCursor(19, 2);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY1, OUTPUT);
  lcd.init();
  lcd.backlight();
}

void loop() {
  lastVolts = volts;
  lastAmps = amps;
  lastWatts = watts;
  volts = readVolts();
  amps = readAmps();
  watts = readPower(volts, amps);

  screen01(volts, amps, watts);
  outputRelay(volts);
  
  delay(1500);
}
