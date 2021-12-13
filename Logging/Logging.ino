#include "Config.h"
#ifdef _HAVE_WIFI
#include <WiFiEsp.h>
#endif
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ACS712.h>

int has_wifi = 0;
int duty_cycle = 128;
float lastVolts = 0.0;
float lastAmps = 0.0;
float lastWatts = 0.0;
float volts = 0.0;
float amps = 0.0;
float watts = 0.0;

LiquidCrystal_I2C lcd(LCD_ADDR,LCD_COLS,LCD_ROWS);
ACS712 acsBatt(IBATT, (float)VREF, ADC_COUNT, ACS712_VAL);
ACS712 acsPanel(IPANL, (float)VREF, ADC_COUNT, ACS712_VAL);

float readBattVolts(void) {
  float scaleFactor = (float)BATTR2 / ((float)BATTR1 + (float)BATTR2);
  int in = analogRead(VBATT);
#ifdef __DEBUG
  Serial.print("readBattVolts: ");
  Serial.println(String(in));
#endif
  return ((float)in / 1024 * VREF) / scaleFactor;
}

float readPanelVolts(void) {
  float scaleFactor = (float)BATTR2 / ((float)BATTR1 + (float)BATTR2);
  int in = analogRead(VPANL);
#ifdef __DEBUG
  Serial.print("readPanelVolts: ");
  Serial.println(String(in));
#endif
  return ((float)in / 1024 * VREF) / scaleFactor;
}

float readBattAmps() {
  return abs((float)acsBatt.mA_DC() / 1000);
}

float readPanelAmps() {
  return abs((float)acsPanel.mA_DC() / 1000);
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

/*
 * This display shows Input vs Output
 */
void screen02(float vIn, float vOut) {
  lcd.setCursor(0, 0);
//  lcd.print("012345678901234567890");
  lcd.print("Volts In:");
  lcd.setCursor(11, 0);
  lcd.print("         ");
  lcd.print(String(vIn));
  lcd.setCursor(0, 1);
  lcd.print("Volts Out:");
  lcd.print("         ");
  lcd.setCursor(11, 1);
  lcd.print(String(vOut));
}

void increasePWM(void)
{
    if (duty_cycle < 255)
        duty_cycle += 1;
}

void decreasePWM(void)
{
    if (duty_cycle > 1)
        duty_cycle -= 1;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
#ifdef _HAVE_WIFI  
  Serial3.begin(115200);
#endif  
  
#ifdef _HAVE_WIFI  
  WiFi.init(&Serial3);
  if(WiFi.status() == WL_NO_SHIELD) {
    Serial.println("No WiFi");
    has_wifi = 0;
  }
  else {
    Serial.println("Found WiFi Module");
    has_wifi = 1;
  }
#endif
  pinMode(RELAY1, OUTPUT);
  pinMode(DRIVE, OUTPUT);
  lcd.init();
  lcd.backlight();
  Serial.println("Volts,Amps,Watts,Duty");
}

void loop() {
  lastVolts = volts;
  lastAmps = amps;
  lastWatts = watts;
  volts = readBattVolts();
  amps = readBattAmps();
  watts = readPower(volts, amps);

  // First work out where we are for power
  if(watts > lastWatts) {
    if(volts > lastVolts) {
      increasePWM();
    }
    else {
      decreasePWM();
    }
  }
  else {
    if(volts > lastVolts) {
      decreasePWM();
    }
    else {
      increasePWM();
    }
  }

#ifdef _DEBUG
  Serial.print(volts);
  Serial.print(",");
  Serial.print(amps);
  Serial.print(",");
  Serial.print(watts);
  Serial.print(",");
  Serial.print(duty_cycle * 100 / 255);
  Serial.println();
#endif
  analogWrite(DRIVE, duty_cycle);

  screen01(volts, amps, watts);
  // In a perfect world, we should add some hysteresis
  // to the the outputRelay function, but for now the
  // 1500mS loop delay should do the job for us.
  outputRelay(volts);
  
  delay(1500);
}
