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

#ifndef MAX17205_H_
#define MAX17205_h_

// === includes ===
#include "drivers.h"

// === Command codes ===
namespace MAX17205 {

    // === Constants ===
    constexpr unsigned char i2c_address_low  = 0x6C; // For accessing registers 0x000 - 0x0FF
    constexpr unsigned char i2c_address_high = 0x16; // For accessing registers 0x180 - 0x1FF
    constexpr unsigned char R_sense_mOhm     = 5;   // [mΩ] The value of the current sense resistor

    // === Bits ===
    // *** nPackCfg ***
    constexpr unsigned int BALCFG_0         = (1 << 5);
    constexpr unsigned int BALCFG_1         = (1 << 6);
    constexpr unsigned int BALCFG_2         = (1 << 7);
    constexpr unsigned int ChEn             = (1 << 10);
    constexpr unsigned int TdEn             = (1 << 11);


    // === Registers ===
    enum class Register: unsigned int
    {
        Current         = 0x00A,
        Avg_Current     = 0x00B,
        PackCfg         = 0x0BD,
        Avg_Cell_4      = 0x0D1,
        Avg_Cell_3      = 0x0D2,
        Avg_Cell_2      = 0x0D3,
        Avg_Cell_1      = 0x0D4,
        Cell_4          = 0x0D5,
        Cell_3          = 0x0D6,
        Cell_2          = 0x0D7,
        Cell_1          = 0x0D8,
        Batt_Register   = 0x0DA,
        nConfig         = 0x1B0,
        nPackCfg        = 0x1B5
    };

    // === Classes ===
    class Controller
    {
    private:
        // *** properties ***
        I2C::Controller_Base* i2c;
        I2C::Data_t     i2c_data;
        unsigned int    voltage_battery = 0;
        signed int      current_battery = 0;
        unsigned int    voltage_cell[2] = {0};
        unsigned int    capacity[2]     = {0};
        unsigned int    cycles          = 0;
        unsigned int    age             = 0;
        unsigned int    esr             = 0;
        signed int      temperature     = 0;    

        // *** methods ***
        static unsigned int    to_voltage       (const unsigned int raw);
        static signed int      to_current       (const unsigned int raw);
        static unsigned int    to_capacity      (const unsigned int raw);
        static unsigned int    to_percentage    (const unsigned int raw);
        static signed int      to_temperature   (const unsigned int raw);
        static unsigned int    to_resistance    (const unsigned int raw);

    public:
        // *** Constructor ***
        Controller(I2C::Controller_Base& i2c_controller);

        // *** Properties

        // *** Methods ***
        bool            initialize              (void);
        unsigned int    get_battery_voltage     (void) const;
        signed int      get_battery_current     (void) const;
        unsigned int    get_cell_voltage        (const unsigned char cell) const;
        unsigned int    get_total_capacity      (void) const;
        unsigned int    get_remaining_capacity  (void) const;
        unsigned int    get_number_cycles       (void) const;
        unsigned int    get_age                 (void) const;
        unsigned int    get_ESR                 (void) const;
        signed int      get_temperature         (void) const;    
        bool            read_register           (const Register reg);
        bool            write_register          (const Register reg, const unsigned int data);
        bool            read_battery_voltage    (void);
        bool            read_battery_current    (void);
        bool            read_battery_current_avg(void);
        bool            read_cell_voltage       (void);
        bool            read_cell_voltage_avg   (void);


    };
};
#endif