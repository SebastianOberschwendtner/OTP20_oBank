/**
 * OTP20 - oBank
 * Copyright (c) 2021 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef BQ25700_H_
#define BQ25700_h_

// === includes ===
#include "drivers.h"

// === Command codes ===
namespace BQ25700 {
    // Fixed device values
    constexpr unsigned char i2c_address     = 0x12;
    constexpr unsigned char manufacturer_id = 0x40;
    constexpr unsigned char device_id       = 0x79;

    // Option bits
    // *** Charge_Option_0 ***
    constexpr unsigned int EN_LWPWR             = (1<<15);
    constexpr unsigned int WDTMR_ADJ_1          = (1<<14);
    constexpr unsigned int WDTMR_ADJ_0          = (1<<13);
    constexpr unsigned int IDPM_AUTO_DISABLE    = (1<<12);
    constexpr unsigned int OTG_ON_CHRGOK        = (1<<11);
    constexpr unsigned int EN_OOA               = (1<<10);
    constexpr unsigned int PWM_FREQ             = (1<<9); // Set Switching frequency
    constexpr unsigned int EN_LEARN             = (1<<5);
    constexpr unsigned int IADPT_GAIN           = (1<<4); // IADPT amplifier ratio
    constexpr unsigned int IBAT_GAIN            = (1<<3); // IBAT amplifier ratio
    constexpr unsigned int EN_LDO               = (1<<2);
    constexpr unsigned int EN_IDPM              = (1<<1); // Enable IDPM regulation loop
    constexpr unsigned int CHRG_INHIBIT         = (1<<0);
    // *** Charge_Option_1 ***
    constexpr unsigned int EN_IBAT              = (1<<15); // Enable IBAT output buffer
    constexpr unsigned int EN_PSYS              = (1<<12); // Enable PSYS sensing circuit and output buffer
    constexpr unsigned int RSNS_RAC             = (1<<11); // Set input sense resistor RAC
    constexpr unsigned int RSNS_RSR             = (1<<10); // set charge sense resistor RSR
    constexpr unsigned int PSYS_RATIO           = (1<<9); // Ratio of PSYS output current vs. total input battery power
    constexpr unsigned int CMP_REF              = (1<<8); // Set independent comparator internal reference
    constexpr unsigned int CMP_POL              = (1<<7); // Set independent comparator polarity
    constexpr unsigned int FORCE_LATCHOFF       = (1<<3); // Force power path off
    constexpr unsigned int EN_SHIP_DCHRG        = (1<<1); // Discharge SRN for shipping mode
    constexpr unsigned int AUTO_WAKEUP_EN       = (1<<0);
    // *** Charge_Option_2 ***
    constexpr unsigned int EN_PKPWR_IDPM        = (1<<13); // Enable peak power mode triggered by input current overshoot
    constexpr unsigned int EN_PKPWR_VSYS        = (1<<12); // Enable peak power mode triggered by system voltage undershoot
    constexpr unsigned int PKPWR_OVLD_STAT      = (1<<11); // Indicator at the device is in overloading cycle
    constexpr unsigned int PKPWR_RELAX_STAT     = (1<<10); // Indicator at the device is in relaxation cycle
    constexpr unsigned int EN_EXTILIM           = (1<<7); // Enable ILIM_HIZ pin to set input current limit
    constexpr unsigned int EN_ICHG_IDCHG        = (1<<6);  // Set function of IBAT pin
    constexpr unsigned int Q2_OCP               = (1<<5); // Q2 OCP threshold by sensing Q2 VDS
    constexpr unsigned int ACX_OCP              = (1<<4); // Input current OCP threshold by sensing ACP-ACN
    constexpr unsigned int EN_ACOC              = (1<<3); // Enable input overcurrent protection by sensing the voltage across ACP-ACN
    constexpr unsigned int ACOC_VTH             = (1<<2); // Set Mosfet OCP threshold as percentage of IDPM
    constexpr unsigned int EN_BATOC             = (1<<1); // Enable battery discharge overcurrent protection by sensing the voltage across SRN-SRP
    constexpr unsigned int BATOC_VTH            = (1<<0); // Set battery discharge overcurrent threshold
    // *** Charge_Option_3 ***
    constexpr unsigned int EN_HIZ               = (1<<15); // Device HI-Z mode enable
    constexpr unsigned int RESET_REG            = (1<<14); // Reset registers
    constexpr unsigned int RESET_VINDPM         = (1<<13); // Reset VINDPM threshold
    constexpr unsigned int EN_OTG               = (1<<12); // Enable device OTG mode when EN_OTG pin is HIGH
    constexpr unsigned int EN_ICO_MODE          = (1<<11); // Enable ICO algorithm
    constexpr unsigned int BATFETOFF_HIZ        = (1<<1); // Control BATFET during HI-Z mode
    constexpr unsigned int PSYS_OTG_IDCHG       = (1<<0); // PSYS function during OTG mode
    // *** Prochot_Option_0 ***
    // *** Prochot_Option_1 ***
    // *** ADC_Option ***
    constexpr unsigned int ADC_CONV             = (1<<15); // Coonversion mode of ADC
    constexpr unsigned int ADC_START            = (1<<14); // Start ADC conversion
    constexpr unsigned int ADC_FULLSCALE        = (1<<13); // ADC input voltage range
    constexpr unsigned int EN_ADC_CMPIN         = (1<<7);
    constexpr unsigned int EN_ADC_VBUS          = (1<<6);
    constexpr unsigned int EN_ADC_PSYS          = (1<<5);
    constexpr unsigned int EN_ADC_IIN           = (1<<4);
    constexpr unsigned int EN_ADC_IDCHG         = (1<<3);
    constexpr unsigned int EN_ADC_ICHG          = (1<<2);
    constexpr unsigned int EN_ADC_VSYS          = (1<<1);
    constexpr unsigned int EN_ADC_VBAT          = (1<<0);
    // *** Charger_Status ***
    constexpr unsigned int AC_STAT              = (1<<15); // Input source status, same as CHRG_OK bit
    constexpr unsigned int ICO_DONE             = (1<<14); // ICO routine successfully executed
    constexpr unsigned int IN_VINDPM            = (1<<12);
    constexpr unsigned int IN_IINDPM            = (1<<11);
    constexpr unsigned int IN_FCHRG             = (1<<10); // Fast charge mode
    constexpr unsigned int IN_PCHRG             = (1<<9); // Pre-charge mode
    constexpr unsigned int IN_OTG               = (1<<8); // OTG mode
    constexpr unsigned int FAULT_ACOV           = (1<<7);
    constexpr unsigned int FAULT_BATOC          = (1<<6);
    constexpr unsigned int FAULT_ACOC           = (1<<5);
    constexpr unsigned int SYSOVP_STAT          = (1<<4); // SYSOVP status and clear
    constexpr unsigned int FAULT_LATCHOFF       = (1<<2);
    constexpr unsigned int FAULT_OTG_OVP        = (1<<1);
    constexpr unsigned int FAULT_OTG_UCP        = (1<<0);
    // *** Prochot_Status ***

    // command registers
    enum class Register: unsigned char
    {
        Charge_Option_0     = 0x12, // R/W
        Charge_Option_1     = 0x30, // R/W
        Charge_Option_2     = 0x31, // R/W
        Charge_Option_3     = 0x32, // R/W
        Charge_Current      = 0x14, // R/W
        Max_Charge_Voltage  = 0x15, // R/W
        Prochot_Option_0    = 0x33, // R/W
        Prochot_Option_1    = 0x34, // R/W
        ADC_Option          = 0x35, // R/W
        Charger_Status      = 0x20, // R
        Prochot_Status      = 0x21, // R
        Input_Limit_DPM     = 0x22, // R Used input current limit value
        ADC_VBUS            = 0x23, // R ADC results of VBUS and PSYS
        ADC_BAT_Current     = 0x24, // R ADC results of battery currents
        ADC_Input_Current   = 0x25, // R ADC results of input current and CMPNI
        ADC_SYS_Voltage     = 0x26, // R ADC results of VSYS and VBAT
        OTG_Voltage         = 0x3B, // R/W
        OTG_Current         = 0x3C, // R/W
        Input_Voltage       = 0x3D, // R/W
        Min_SYS_Voltage     = 0x3E, // R/W
        Input_Limit_Host    = 0x3F, // R/W Set input current limit
        Manufacturer_ID     = 0xFE, // R/W
        Device_ID           = 0xFF // R/W
    };

    // Controller states
    enum class State: unsigned char
    {
        Init = 0, Idle, Charging, OTG, Error
    };


    // === Classes ===
    class Controller
    {
    private:
        // *** properties ***
        I2C::Controller_Base* i2c;
        I2C::Data_t i2c_data;
        State state = State::Init;
        unsigned int voltage_system = 0;
        unsigned int voltage_input  = 0;
        signed int   current_input  = 0;
        unsigned int voltage_OTG    = 0;
        unsigned int current_OTG    = 0;
        unsigned int current_charge = 0;

        // *** methods ***
        // bool        send_command_byte(const unsigned char cmd);
        bool         read_register(const Register reg);

    public:
        // *** Constructor ***
        Controller(I2C::Controller_Base& i2c_controller);

        // *** Properties

        // *** Methods ***
        unsigned int    get_system_voltage  (void) { return this->voltage_system; };
        unsigned int    get_input_voltage   (void) { return this->voltage_input; };
        signed int      get_input_current   (void) { return this->current_input; };
        unsigned int    get_OTG_voltage     (void) { return this->voltage_OTG + 4480; };
        unsigned int    get_OTG_current     (void) { return this->current_OTG; };
        unsigned int    get_charge_current  (void) { return this->current_charge; };
        bool            initialize          (void);
        State           get_state           (void) const;
        bool            write_register      (const Register reg, const unsigned int data);
        bool            set_charge_current  (const unsigned int current);
        bool            set_OTG_voltage     (const unsigned int voltage);
        bool            set_OTG_current     (const unsigned int current);
        bool            enable_OTG          (const bool state);

    };
};

#endif