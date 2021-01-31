/**
 * @file    MPR121.h
 * @brief   Device driver - MPR121 capactiive touch IC
 * @author  sam grove
 * @version 1.0
 * @see     http://cache.freescale.com/files/sensors/doc/data_sheet/MPR121.pdf
 *
 * Copyright (c) 2013
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MPR121_H
#define __MPR121_H

#include <mbed.h>
#include <OK_I2C.h>


/**
 *  @class MPR121
 *  @brief API for the MPR121 capacitive touch IC
 */
class MPR121 : public OK_I2C {
private:
    InterruptIn irq;
    volatile uint16_t _button;
    volatile uint32_t _button_has_changed;

    /** The interrupt handler for the IRQ pin
     */
    void irq_handler(void);

public:
    /**
     *  @enum MPR121_ADDR
     *  @brief Possible terminations for the ADDR pin
     */
    enum MPR121_ADDR
    {
        ADDR_VSS = 0x5A, /*!< ADDR connected to VSS */
        ADDR_VDD,        /*!< ADDR connected to VDD */
        ADDR_SCL,        /*!< ADDR connected to SDA */
        ADDR_SDA         /*!< ADDR connected to SCL */
    };

    /** Create the MPR121 object
     *  @param i2c - A defined I2C object
     *  @param pin - A defined InterruptIn object
     *  @param i2c_addr - Connection of the address line
     */
    MPR121(I2C *i2c_ptr, PinName irq_pin, MPR121_ADDR i2c_addr = ADDR_VSS) : irq(irq_pin) {
        address = i2c_addr << 1;
        i2c = i2c_ptr;
    }

    /** Clear state variables and initilize the dependant objects
     */
    void init(void);

    /** Allow the IC to run and collect user input
     */
    void enable(void);

    /** Stop the IC and put into low power mode
     */
    void disable(void);

    /** Determine if a new button press event occured
     *  Upon calling the state is cleared until another press is detected
     *  @return 1 if a press has been detected since the last call, 0 otherwise
     */
    uint32_t isPressed(void);

    /** Get the electrode status (ELE12 ... ELE0 -> b15 xxx b11 ... b0
     *  The buttons are bit mapped. ELE0 = b0 ... ELE11 = b11 b12 ... b15 undefined
     *  @return The state of all buttons
     */
    uint16_t buttonPressed(void);
    uint16_t getTouched();
    bool wasTouched();
    bool wasReleased();

    /**
     *  @enum MPR121_REGISTER
     *  @brief The device register map
     */
    enum MPR121_REGISTER
    {
        ELE0_7_STAT = 0x00, // ELE0 - ELE7 Touch Status
        ELE8_11_STAT,       // ELE8 - ELE11, ELEPROX Touch Status
        ELE0_7_OOR_STAT,    // ELE0-7 OOR Status
        ELE8_11_OOR_STAT,   // ELE8-11, ELEPROX OOR Status

        EFD0LB, // ELE0 Electrode Filtered Data LSB
        EFD0HB, // ELE0 Electrode Filtered Data MSB
        EFD1LB,
        EFD1HB,
        EFD2LB,
        EFD2HB,
        EFD3LB,
        EFD3HB,
        EFD4LB,
        EFD4HB,
        EFD5LB,
        EFD5HB,

        EFD6LB = 0x10,
        EFD6HB,
        EFD7LB,
        EFD7HB,
        EFD8LB,
        EFD8HB,
        EFD9LB,
        EFD9HB,
        EFD10LB,
        EFD10HB,
        EFD11LB,
        EFD11HB,
        EFDPROXLB, // ELEPROX Electrode Filtered Data LSB
        EFDPROXHB, // ELEPROX Electrode Filtered Data MSB

        E0BV, // ELE0 Baseline Value
        E1BV,
        E2BV = 0x20,
        E3BV,
        E4BV,
        E5BV,
        E6BV,
        E7BV,
        E8BV,
        E9BV,
        E10BV,
        E11BV,
        EPROXBV, // ELEPROX Baseline Value

        MHDR,        // MHD Rising
        NHDR,        // NHD Amount Rising
        NCLR,        // NCL Rising
        FDLR,        // FDL Rising
        MHDF,        // MHD Falling
        NHDF = 0x30, // NHD Amount Falling
        NCLF,        // NCL Falling
        FDLF,        // FDL Falling
        NHDT,        // NHD Amount Touched
        NCLT,        // NCL Touched
        FDLT,        // FDL Touched

        MHDPROXR,        // ELEPROX MHD Rising
        NHDPROXR,        // ELEPROX NHD Amount Rising
        NCLPROXR,        // ELEPROX NCL Rising
        FDLPROXR,        // ELEPROX FDL Rising
        MHDPROXF,        // ELEPROX MHD Falling
        NHDPROXF,        // ELEPROX NHD Amount Falling
        NCLPROXF,        // ELEPROX NCL Falling
        FDLPROXF,        // ELEPROX FDL Falling
        NHDPROXT,        // ELEPROX NHD Amount Touched
        NCLPROXT,        // ELEPROX NCL Touched
        FDLPROXT = 0x40, // ELEPROX FDL Touched

        E0TTH, // ELE0 Touch Threshold
        E0RTH, // ELE0 Release Threshold
        E1TTH,
        E1RTH,
        E2TTH,
        E2RTH,
        E3TTH,
        E3RTH,
        E4TTH,
        E4RTH,
        E5TTH,
        E5RTH,
        E6TTH,
        E6RTH,
        E7TTH,
        E7RTH = 0x50,
        E8TTH,
        E8RTH,
        E9TTH,
        E9RTH,
        E10TTH,
        E10RTH,
        E11TTH,
        E11RTH,
        EPROXTTH, // ELEPROX Touch Threshold
        EPROXRTH, // ELEPROX Release Threshold

        DT_DR,      // Debounce Touch & Release
        CDC_CONFIG, // Filter/Global CDC Configuration
        CDT_CONFIG, // Filter/Global CDT Configuration
        ECR,        // Electrode Configuration

        CDC0, // ELE0 Electrode Current
        CDC1 = 0x60,
        CDC2,
        CDC3,
        CDC4,
        CDC5,
        CDC6,
        CDC7,
        CDC8,
        CDC9,
        CDC10,
        CDC11,
        CDCPROX, // ELEPROX Electrode Current

        CDT0_CDT1, // ELE0, ELE1 Charge Time
        CDT2_CDT3,
        CDT4_CDT5,
        CDT6_CDT7,
        CDT8_CDT9 = 0x70,
        CDT10_CDT11,
        CDTPROX, // ELEPROX Charge Time

        GPIO_CTRL0,  // GPIO Control Register 0
        GPIO_CTRL1,  // GPIO Control Register 1
        GPIO_DATA,   // GPIO Data Register
        GPIO_DIR,    // GPIO Direction Register
        GPIO_EN,     // GPIO Enable Register
        GPIO_SET,    // GPIO Data Set Register
        GPIO_CLR,    // GPIO Data Clear Register
        GPIO_TOGGLE, // GPIO Data Toggle Register

        AUTO_CFG0, // AUTO-CONFIG Control Register 0
        AUTO_CFG1, // AUTO-CONFIG Control Register 1
        USL,       // AUTO-CONFIG USL Register
        LSL,       // AUTO-CONFIG LSL Register
        TL,        // AUTO-CONFIG Target Level Register

        SRST = 0x80 // Soft Reset Register
    };
};

#endif
