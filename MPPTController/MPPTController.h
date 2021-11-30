#define _DEBUG
#define VERSION "0.0.1"

#define ENC_A 2
#define ENC_B 3
#define ENC_BUTTON 4
#define DRIVE 5
#define RELAY 7
#define VOUT A0
#define IOUT A1

// Multiplier to get ADC reading in the actual units
#define VOLTAGE_SCALE 0.0278505 // Assuming max Vin = 25V (via a divider of course!)
#define CURRENT_SCALE 0.0048828125

float lastPower = 0.0;
float currentPower = 0.0;
float lastVoltage = 0.0;
float volts = 0.0;
float amps = 0.0;
// 50% as a starting value
uint16_t powerOut = 128;
