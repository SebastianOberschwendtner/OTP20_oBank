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

#ifndef SSD1306_H_
#define SSD1306_h_

// === includes ===
#include "drivers.h"

// === Command codes ===
namespace SSD1306 {
    constexpr unsigned char i2c_address         = 0x3C<<1; // 011110+SA0+RW - 0x3C or 0x3D

    // commands
    enum class Command: unsigned char
    {
        set_contrast        = 0x81,
        display_ram         = 0xA4,
        display_all_on      = 0xA5,
        display_normal      = 0xA6,
        display_inverted    = 0xA7,
        display_off         = 0xAE,
        display_on          = 0xAF,
        set_display_offset  = 0xD3,
        set_com_pins        = 0xDA,
        set_vcom_detect     = 0xDB,
        set_disp_clock_div  = 0xD5,
        set_precharge       = 0xD9,
        set_multiplex       = 0xA8,
        set_column_low      = 0x00,
        set_column_high     = 0x10,
        set_startline       = 0x40,
        memory_mode         = 0x20,
        column_address      = 0x21,
        page_address        = 0x22,
        com_scan_inc        = 0xC0,
        com_scan_dec        = 0xC8,
        seg_remap           = 0xA0,
        charge_pump         = 0x8D,
        external_vcc        = 0x01,
        switch_cap_vcc      = 0x02,
    
        // Scrolling
        scroll_activate     = 0x2F,
        scroll_deactivate   = 0x2E,
        scroll_set_vertical = 0xA3,
        scroll_hori_right   = 0x26,
        scroll_hori_left    = 0x27,
        scroll_vert_hori_left  = 0x29,
        scroll_vert_hori_right = 0x2A
    };

    // === Classes ===
    class Controller
    {
    private:
        // *** properties ***
        I2C::Controller_Base* i2c;
        I2C::Data_t payload;

        // *** methods ***
        bool        send_command_byte(const Command cmd);
        bool        send_command_data(const unsigned char cmd);

    public:
        // *** Constructor ***
        Controller(I2C::Controller_Base& i2c_controller);

        // *** Methods ***

        bool        initialize  (void);
        bool        on          (void);
        bool        off         (void);
        bool        draw        (const unsigned char* buffer);
    };
};

#endif