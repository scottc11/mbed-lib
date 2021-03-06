#ifndef __TLC59116_H
#define __TLC59116_H

#include "mbed.h"
#include "BitwiseMethods.h"

#define TLC59116_DEFAULT_ADDR   1100000
#define TLC59116_ALL_CALL       1101000

class TLC59116 {
public:
  
  enum LedState {
    OFF = 0,    // LED is off (default power-up state)
    ON = 1,     // LED is fully on (individual brightness and group dimming/blinking not controlled)
    PWM = 2,    // LED is individual brightness can be controlled through its PWMx register
    GROUP = 3,  // LED individual brightness and group dimming/blinking can be controlled through its PWMx register and the GRPPWM registers
  };

  TLC59116(I2C * _i2c, int _address = TLC59116_DEFAULT_ADDR) {
    i2c = _i2c;
    address = _address << 1;
  };
  
  int address;
  I2C *i2c;
  int buffer;
  uint8_t led0States;
  uint8_t led1States;
  uint8_t led2States;
  uint8_t led3States;

  void initialize();
  void enableDimmingMode();
  void enableBlinkingMode();
  void setMode(int mode1Value, int mode2Value);
  void setVoltageGain(int gain);
  void setLedOutput(int led, LedState state, int pwm=0);
  void setLedOutput16(uint16_t value);
  void setBiColorLedOutput(int pins, int state);
  void setLedPWM(int led, int value);
  void setGroupPWM(int value);
  void setGroupFREQ(int value);
  void setAllOutputsLow();
  void setAllOutputsHigh();

private:
    
  void writeRegister(char reg, char data) {    
    char buffer[2];
    buffer[0] = reg;
    buffer[1] = data;
    // i2c->start();
    i2c->write(address, buffer, 2);
    // i2c->stop();
  }

  char readRegister(char reg) {
    char command[1];
    char buffer[1];
    command[0] = reg;
    i2c->write(address, command, 1, true);
    i2c->read(address, buffer, 1);
    return buffer[0];
  }

  enum Registers {
    MODE1 = 0x00, // R/W Mode 1
    MODE2 = 0x01, // R/W Mode 2
    PWM0 = 0x02,  // R/W Brightness control LED0
    PWM1 = 0x03,  // R/W Brightness control LED1
    PWM2 = 0x04,  // R/W Brightness control LED2
    PWM3 = 0x05,  // R/W Brightness control LED3
    PWM4 = 0x06,  // R/W Brightness control LED4
    PWM5 = 0x07,  // R/W Brightness control LED5
    PWM6 = 0x08,  // R/W Brightness control LED6
    PWM7 = 0x09,  // R/W Brightness control LED7
    PWM8 = 0x0A,  // R/W Brightness control LED8
    PWM9 = 0x0B,  // R/W Brightness control LED9
    PWM10 = 0x0C, // R/W Brightness control LED10
    PWM11 = 0x0D, // R/W Brightness control LED11
    PWM12 = 0x0E, // R/W Brightness control LED12
    PWM13 = 0x0F, // R/W Brightness control LED13
    PWM14 = 0x10, // R/W Brightness control LED14
    PWM15 = 0x11, // R/W Brightness control LED15
    GRPPWM = 0x12, // R/W Group duty cycle control
    GRPFREQ = 0x13, // R/W Group frequency
    LEDOUT0 = 0x14, // R/W LED output state 0
    LEDOUT1 = 0x15, // R/W LED output state 1
    LEDOUT2 = 0x16, // R/W LED output state 2
    LEDOUT3 = 0x17, // R/W LED output state 3
    SUBADR1 = 0x18, // R/W I2C bus subaddress 1
    SUBADR2 = 0x19, // R/W I2C bus subaddress 2
    SUBADR3 = 0x1A, // R/W I2C bus subaddress 3
    ALLCALLADR = 0x1B, // R/W LED All Call I2C bus address
    IREF = 0x1C, // R/W IREF configuration
    EFLAG1 = 0x1D, // R Error flags 1
    EFLAG2 = 0x1E, // R Error flags 2
  };

  char pwmRegisters[16] = { PWM0, PWM1, PWM2, PWM3, PWM4, PWM5, PWM6, PWM7, PWM8, PWM9, PWM10, PWM11, PWM12, PWM13, PWM14, PWM15 };
};


#endif