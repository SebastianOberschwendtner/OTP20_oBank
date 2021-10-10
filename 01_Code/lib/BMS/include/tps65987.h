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

#ifndef TPS65987_H_
#define TPS65987_h_

// === includes ===
#include "drivers.h"
#include <array>

// === Command codes ===
namespace TPS65987 {

    // === Constants ===
    constexpr unsigned char i2c_address         = 0x40;

    // === Bits ===
    constexpr unsigned char PlugDetails_0       = (1<<0);
    constexpr unsigned char PlugDetails_1       = (1<<1);
    constexpr unsigned char CCPullUp_0          = (1<<2);
    constexpr unsigned char CCPullUp_1          = (1<<3);
    constexpr unsigned char PortType_0          = (1<<4);
    constexpr unsigned char PortType_1          = (1<<5);
    constexpr unsigned char PresentRole         = (1<<6);

    // === Registers ===
    struct reg_t
    {
        unsigned char address;
        unsigned char length;
    };

    namespace Register
    {
        constexpr reg_t Mode        = {0x03, 4};
        constexpr reg_t Cmd1        = {0x08, 4};
        constexpr reg_t Data1       = {0x09, 64};   
        constexpr reg_t TX_Sink_Cap = {0x33, 57};
        constexpr reg_t Active_PDO  = {0x34, 6};
        constexpr reg_t Active_RDO  = {0x35, 4};
        constexpr reg_t PD_Status   = {0x40, 4};
    }; // namespace Register

    // === Modes ===
    enum class Mode: unsigned char
    {
        BOOT = 0, PTCH, APP, OTHER
    };

    // === Power contracts ===
    struct Contract
    {
        unsigned char role;
        unsigned char USB_type;
        unsigned int  voltage;
        unsigned int  current; 
    };
     
    class Controller
    {
    private:
        // *** properties ***
        I2C::Controller_Base*         i2c;
        I2C::Data_t                   i2c_data          = {0};
        std::array<char, 6>           buffer_cmd        = {0x08, 0x04, 0, 0, 0, 0};
        std::array<unsigned char, 66> buffer_data       = {0};
        Mode                          mode_active       = Mode::BOOT;
        mutable char                  cmd_active[4]     = {0};
        Contract                      contract_active   = {0, 0, 0, 0};

    public:
        // *** Constructor ***
        Controller(I2C::Controller_Base& i2c_controller);

        // Properties

        // *** Methods ***
        bool            initialize          (void);
        Mode            get_mode            (void) const { return this->mode_active; };
        char*           get_active_command  (void) const { return &this->cmd_active[0]; };
        Contract        get_active_contract (void) const { return this->contract_active; };
        bool            read_register       (const reg_t reg);
        bool            write_register      (const reg_t reg);
        bool            read_active_command (void);
        bool            write_command       (const char* cmd);
        bool            read_mode           (void);
        bool            read_PD_status      (void);
    };
};
#endif
