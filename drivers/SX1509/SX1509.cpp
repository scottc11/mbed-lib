
#include "SX1509.h"

void SX1509::init(bool extClock /*false*/) {

  this->reset();
  wait_ms(1);
  int clockConfig = extClock ? 0b00100000 : 0b01010010;  // use either internal or external clock signal
  //  fOSCOUT = fOSC/(2^(RegClock[3:0]-1))
  //  ClkX = fOSC/(2^(RegMisc[6:4]-1))
  this->i2cWrite(REG_CLOCK, clockConfig);
  int regMiscConfig = (DriverMode::LINEAR | ClockSpeed::EXTRA_SLOW);
  regMiscConfig |= 0b00000100;
  this->i2cWrite(REG_MISC, regMiscConfig); // very important for LED Driver mode that this config gets set
}


void SX1509::pinMode(int pin, PinMode mode, bool invertPolarity /*false*/) {
  switch (mode)
  {
    case INPUT:
      setDirection(pin, 1);
      enablePullup(pin);
      if (invertPolarity) { setPolarity(pin, invertPolarity); }
      break;
    case OUTPUT:
      setDirection(pin, 0);
      break;
    case ANALOG_OUTPUT:
      ledConfig(pin);
      break;
  }
}

/**
 * Enables polarity inversion for each IO
 * 0 : Normal polarity
 * 1 : Inverted polarity
 */
void SX1509::setPolarity(int pin, int polarity) {
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_POLARITY_B + bank;
  this->i2cWrite(reg, bitWrite(this->i2cRead(reg), pinPos, polarity));
}

/**
 * An interrupt NINT can be generated on any programmed combination of I/Os rising and/or falling edges through the RegInterruptMask and RegSense registers.
 * If needed, the I/Os which triggered the interrupt can then be identified by reading RegInterruptSource register.
 * 
 * 0 : An event on this IO will trigger an interrupt
 * 1 : An event on this IO will NOT trigger an interrupt
 * 
 * We enable interrupt on I/O[1] in RegInterruptMask
 * We set edge sense for I/O[1] in RegSense
 * 
 * 00 : None
 * 01 : Rising
 * 10 : Falling
 * 11 : Both
*/

void SX1509::setInterupt(int pin, bool willNotInterupt, IntType type)
{
  int hotValue;
  int mask;
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_INTERRUPT_MASK_B + bank;
  this->i2cWrite(reg, bitWrite(this->i2cRead(reg), pinPos, willNotInterupt));

  // Set Sense
  if (pin > 7) {              // Bank B IOs
    if (pinPos > 3) {         // Bank B pins 4:7
      reg = REG_SENSE_HIGH_B;
      hotValue = this->i2cRead(reg);
      mask = 0b00000011 << (pinPos - 4) * 2; // shift by twos
      hotValue &= ~mask;
      hotValue |= type << (pinPos - 4) * 2;
      this->i2cWrite(reg, hotValue);
    } else {                  // Bank B pins 4:7
      reg = REG_SENSE_LOW_B;
      hotValue = this->i2cRead(reg);
      mask = 0b00000011 << pinPos * 2; // shift by twos
      hotValue &= ~mask;
      hotValue |= type << pinPos * 2;
      this->i2cWrite(reg, hotValue);
    }
  } else {
    if (pinPos > 3) { // Bank A pins 4:7
      reg = REG_SENSE_HIGH_A;
      hotValue = this->i2cRead(reg);
      mask = 0b00000011 << (pinPos - 4) * 2; // shift by twos
      hotValue &= ~mask;
      hotValue |= type << (pinPos - 4) * 2;
      this->i2cWrite(reg, hotValue);
    }
    else { // Bank A pins 4:7
      reg = REG_SENSE_LOW_A;
      hotValue = this->i2cRead(reg);
      mask = 0b00000011 << pinPos * 2; // shift by twos
      hotValue &= ~mask;
      hotValue |= type << pinPos * 2;
      this->i2cWrite(reg, hotValue);
    }
  }
}

void SX1509::enableInterupt(int pin, IntType type)
{
  this->setInterupt(pin, false, type);
}

void SX1509::disableInterupt(int pin)
{
  this->setInterupt(pin, true, IntType::NONE);
}

/**
 * value table
  0: 0.5ms x 2MHz/fOSC
  1: 1ms x 2MHz/fOSC
  2: 2ms x 2MHz/fOSC 
  3: 4ms x 2MHz/fOSC
  4: 8ms x 2MHz/fOSC
  5: 16ms x 2MHz/fOSC
  6: 32ms x 2MHz/fOSC
  7: 64ms x 2MHz/fOSC
 
 * Each input can be individually debounced by setting corresponding bits in RegDebounce register.
 * At power up the debounce function is disabled. After enabling the debouncer, the change of the input value 
 * is accepted only if the input value is identical at two consecutive sampling times.
 * 
 * The debounce time common to all IOs can be set in RegDebounceConfig register from 0.5 to 64ms (fOSC = 2MHz).
 
*/
void SX1509::setInputDebounce(int value)
{
  this->i2cWrite(REG_DEBOUNCE_CONFIG, value);
}

/** Software Reset
 * Writing consecutively 0x12 and 0x34 to RegReset register will reset all registers to their default values.
*/
void SX1509::reset() {
  this->i2cWrite(REG_RESET, 0x12);
  this->i2cWrite(REG_RESET, 0x34);
}

/** LED Driver Configuration

Please note that in this configuration the IO must be programmed as open drain output (RegOpenDrain)
with no pull-up (RegPullUp) and input buffer must be disabled (RegInputBufferDisable).

Below are the steps required to use the LED driver with the typical LED connection described §6.2:

- Disable input buffer (RegInputDisable)
- Disable pull-up (RegPullUp)
- Enable open drain (RegOpenDrain)
- Set direction to output (RegDir) – by default RegData is set high => LED OFF
- Enable oscillator (RegClock)
- Configure LED driver clock and mode if relevant (RegMisc)
- Enable LED driver operation (RegLEDDriverEnable)
- Configure LED driver parameters (RegTOn, RegIOn, RegOff, RegTRise, RegTFall)
- Set RegData bit low => LED driver started 
*/

void SX1509::ledConfig(int pin)
{
  int hotConfig; // the current value of the register
  int reg;       // register to read/write
  int bank = getBank(pin);
  int pinNum = getPinPos(pin);

  reg = REG_INPUT_DISABLE_B + bank;
  hotConfig = this->i2cRead(reg);
  this->i2cWrite(reg, bitWrite(hotConfig, pinNum, 1)); // Disable the input buffer (HIGH)

  this->disablePullup(pin); // remove internal pull-up (LOW)
  
  reg = REG_OPEN_DRAIN_B + bank;
  hotConfig = this->i2cRead(reg);
  this->i2cWrite(reg, bitWrite(hotConfig, pinNum, 1)); // set IO to open drain (HIGH)

  this->setDirection(pin, 0); // Set IO direction to output (LOW)

  reg = REG_LED_DRIVER_ENABLE_B + bank;
  hotConfig = this->i2cRead(reg);
  this->i2cWrite(reg, bitWrite(hotConfig, pinNum, 1));

  this->digitalWrite(pin, 0); // setting data LOW means LED Driver started
}

void SX1509::analogWrite(int value) {

  if (value == 1) {
    this->i2cWrite(REG_DATA_B, 0x00); 
  } else {
    this->i2cWrite(REG_DATA_B, 0xFF);
  }
}

void SX1509::digitalWrite(int pin, int value) {
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_DATA_B + bank;
  this->i2cWrite(reg, bitWrite(this->i2cRead(reg), pinPos, value));
}

int SX1509::digitalRead(int pin)
{
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_DATA_B + bank;
  int data = this->i2cRead(reg);
  return bitRead(data, pinPos);
}

/**
 * 0 : IO is configured as an output
 * 1 : IO is configured as an input
*/
void SX1509::setDirection(int pin, int inOut)
{
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_DIR_B + bank;
  this->i2cWrite(reg, bitWrite(this->i2cRead(reg), pinPos, inOut));
}

void SX1509::enablePullup(int pin)
{
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_PULL_UP_B + bank;
  this->i2cWrite(reg, bitWrite(this->i2cRead(reg), pinPos, 1));
}

void SX1509::disablePullup(int pin)
{
  int bank = getBank(pin);
  int pinPos = getPinPos(pin);
  int reg = REG_PULL_UP_B + bank;
  this->i2cWrite(reg, bitWrite(this->i2cRead(reg), pinPos, 0));
}

/** LED PWM
 * ON Intensity of IO[X]
 * 
 * Linear mode : IOnX = RegIOnX
 * Logarithmic mode (fading capable IOs only) : IOnX = f(RegIOnX)
*/
void SX1509::setPWM(int pin, int value)
{
  this->i2cWrite(REG_I_ON[pin], value); // sets the PWM / brightness
}

/** LED Driver Blink Mode
 * 
 * Invoked when TOnX != 0 and TOffX != 0. (they both default to 0x00)
 * If the I/O doesn’t support fading the LED intensity will step directly to the IOnX/IOffX value.
 * When RegData(X) is cleared, the LED will complete any current ramp, and then stay at minimum intensity

TOnX:
0 : Infinite (Static mode, TOn directly controlled by RegData, Cf §4.8.2)
1 - 15 : TOnX = 64 * RegTOnX * (255/ClkX)
16 - 31 : TOnX = 512 * RegTOnX * (255/ClkX)

ToffX:


*/
void SX1509::setBlink(int pin, uint8_t onTime, uint8_t offTime, uint8_t onIntensity, uint8_t offIntensity)
{
  this->i2cWrite(REG_T_ON[pin], (onTime > 31) ? 31 : onTime);
  this->setPWM(pin, onIntensity);
  
  uint8_t offValue = offTime << 3; // offTime is 5 bits, from bit 7:3
  offValue |= (offIntensity & 0x07);      // offIntensity is a 3 bit number, from bit 2:0
  this->i2cWrite(REG_OFF[pin], offValue);
}

/**
 * implementation not fully complete. 
 */
void SX1509::setDriverMode(bool linear)
{
  int hotValue = this->i2cRead(REG_MISC);
  if (linear) {
    hotValue = bitClear(hotValue, 7);
  }
  else {
    hotValue = bitSet(hotValue, 7);
  }
  this->i2cWrite(REG_MISC, hotValue);
};

// configure Clock

/**

Bit 7 = unused

Oscillator frequency (fOSC) source (bits 6:5)
00 : OFF. LED driver, keypad engine and debounce features are disabled.
01 : External clock input (OSCIN)
10 : Internal 2MHz oscillator
11 : Reserved

OSCIO pin function (bit 4)
0 : OSCIO is an input (OSCIN)
1 : OSCIO is an output (OSCOUT)

Frequency of the signal output on OSCOUT pin (bits 3:0):
0x0 : 0Hz, permanent "0" logical level (GPO)
0xF : 0Hz, permanent "1" logical level (GPO)
Else : fOSCOUT = fOSC/(2^(RegClock[3:0]-1))

*/

/* Input Debouncer

Each input can be individually debounced by setting corresponding bits in RegDebounce register. At power up the debounce function is disabled. After enabling the debouncer, the change of the input value is accepted only if the input value is identical at two consecutive sampling times.
The debounce time common to all IOs can be set in RegDebounceConfig register from 0.5 to 64ms (fOSC = 2MHz)
*/