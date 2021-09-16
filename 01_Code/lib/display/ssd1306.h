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
    constexpr unsigned char set_contrast        = 0x81;
    constexpr unsigned char display_ram         = 0xA4;
    constexpr unsigned char display_all_on      = 0xA5;
    constexpr unsigned char display_normal      = 0xA6;
    constexpr unsigned char display_inverted    = 0xA7;
    constexpr unsigned char display_off         = 0xAE;
    constexpr unsigned char display_on          = 0xAF;
    constexpr unsigned char set_display_offset  = 0xD3;
    constexpr unsigned char set_com_pins        = 0xDA;
    constexpr unsigned char set_vcom_detect     = 0xDB;
    constexpr unsigned char set_disp_clock_div  = 0xD5;
    constexpr unsigned char set_precharge       = 0xD9;
    constexpr unsigned char set_multiplex       = 0xA8;
    constexpr unsigned char set_column_low      = 0x00;
    constexpr unsigned char set_column_high     = 0x10;
    constexpr unsigned char set_startline       = 0x40;
    constexpr unsigned char memory_mode         = 0x20;
    constexpr unsigned char column_address      = 0x21;
    constexpr unsigned char page_address        = 0x22;
    constexpr unsigned char com_scan_inc        = 0xC0;
    constexpr unsigned char com_scan_dec        = 0xC8;
    constexpr unsigned char seg_remap           = 0xA0;
    constexpr unsigned char charge_pump         = 0x8D;
    constexpr unsigned char external_vcc        = 0x01;
    constexpr unsigned char switch_cap_vcc      = 0x02;

    // Scrolling
    constexpr unsigned char scroll_activate     = 0x2F;
    constexpr unsigned char scroll_deactivate   = 0x2E;
    constexpr unsigned char scroll_set_vertical = 0xA3;
    constexpr unsigned char scroll_hori_right   = 0x26;
    constexpr unsigned char scroll_hori_left    = 0x27;
    constexpr unsigned char scroll_vert_hori_left  = 0x29;
    constexpr unsigned char scroll_vert_hori_right = 0x2A;

    // === Classes ===
    class Controller
    {
    private:
        // *** properties ***
        I2C::Controller_Base* i2c;
        I2C::Data_t payload;

        // *** methods ***
        bool        send_command_byte(const unsigned char cmd);

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