/**
 * OTP20 - oBank
 * Copyright (c) 2022 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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

#ifndef INTERPROCESS_H_
#define INTERPROCESS_H_

// === Includes ===
#include "ipc.h"

// === Inter-process communication ===
namespace IPC
{

    // === Define the PIDs of the tasks ===
    enum PID : unsigned char
    {
        System = 0,
        Display = 1,
        BMS = 2, // Battery Management
        PD = 3,  // Power Delivery Controller
        GUI = 4  // GUI Interface
    };

    // ===  Data interface to the BMS task ===
    struct BMS_Interface
    {
        void sleep(void);
        void wake(void);
        unsigned int get_battery_voltage(void) const;
        signed int get_battery_current(void) const;
        unsigned int get_cell_voltage(unsigned char cell) const;
        unsigned int get_remaining_capacity(void) const;
        unsigned int get_soc(void) const;
        unsigned int get_time2empty(void) const;
        unsigned int get_time2full(void) const;
        bool is_charging(void) const;
    };

    // ===  Data interface to the PD task ===
    struct PD_Interface
    {
        std::array<unsigned long, 4> debug{0};
        void sleep(void);
        void wake(void);
    };

    // ===  Data interface to the GUI task ===
    struct Display_Interface
    {
        void sleep(void);
        void wake(void);
        void next_page(void);
    };
};
#endif