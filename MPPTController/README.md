# MPPT Controller
Simple Arduino-based MPPT Solar Controller

This is the beginnings of a proper charge controller, and it will grow over time.

Schematic is located on my EasyEDA pages.

## REQUIRED LIBRARIES
ACS712
LiquidCrystal_I2C
Wire

## FUNCTIONS COMPLETED
The basic algorithm (Peturb and Observe) is implemented and has been validated on a small panel.  The software assumes the maximum voltage is 25V.  If this is not the case, you will need to change the value of VOLTAGE_SCALE in MPPTController.h

The LCD panel displays power output and PWM percentage.

The rotary encoder is not yet implemented.

A second analog channel for pre-controller voltage is not yet implemented, but allowed for.

SPI is allowed for, but not required.

## TESTING
You can test the software without building the MOSFET circuit if you wish, but you will need the divider network and all the other sensors.

Before connecting the battery or panel to the arduino, calibrate the divider network.  You will need a battery and a voltmeter to do this.  First measure the battery voltage with the divider network connected.  Now adjust the trim resistor so that the voltage output is vBattery / 25 * 5  So if your battery is 12.0 volts, you want to set the network for 2.40 volts.

With this done, connect the network to the Arduino.  The connection from the solar panel to the battery must go via the ACS712 module.

Start up the controller.  You should observe the PWM rate slowly increase to 100%, although it may vary both up and down over time.

Once you're happy this is working correctly, connect the MOSFET circuit, and test again.  You should observe that the PWM value will hover around a value (probably 70-80%) while the sunlight remains constant.  Keep a check on the MOSFET itself as all the charge current flows through there.  It will get hot, and you may need a bigger heatsink.

## FUTURE IMPROVEMENTS
Data logging either to SD Card, or better still over a network is next on the priority list.

Update the software to use the panel cut-off relay when the input voltage drops below a threshold.  This will use the VIN Analog channel which is already allocated.

Use the Rotary Encoder to select displayed parameters.

Monitor MOSFET temperature.

