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
/**
 ******************************************************************************
 * @file    tps65987.cpp
 * @author  SO
 * @version v1.0.0
 * @date    20-September-2021
 * @brief   Driver for the TPS65987DDH(K) USB-3 PD controller.
 ******************************************************************************
 */

// === Includes ===
#include "tps65987.h"

// === Functions ===

/**
 * @brief Constructor for PD controller.
 * @param i2c_controller The reference to the used i2c peripheral.
 */
TPS65987::Controller::Controller(I2C::Controller_Base& i2c_controller):
i2c(&i2c_controller)
{
    
};

/**
 * @brief Initialize the PD IC based on its current mode.
 * @return Returns True when the IC was initialized successfully.
 */
bool TPS65987::Controller::initialize(void)
{
    // First read the current mode of the IC
    if(!this->read_mode()) return false;

    // When in Patch mode, exit this mode
    if (this->get_mode() == Mode::PTCH)
    {
        // Write command
        if (!this->write_command("PTCc")) return false;
    }

    return true;
};

/**
 * @brief Read a register of the PD controller.
 * The function takes the specific length of the register into account.
 * @param reg The register to read from.
 * @return Returns True when the register was read successfully.
 */
bool TPS65987::Controller::read_register(const reg_t reg)
{
    // Set the target address
    this->i2c->set_target_address(i2c_address);

    // Send the data buffer to the register
    this->buffer_data[0] = reg.address;
    return this->i2c->read_array(reg.address, &this->buffer_data[1], reg.length + 1);
};

/**
 * @brief Write a register of the PD controller.
 * The function takes the specific length of the register into account.
 * @param reg The register to write to.
 * @return Returns True when the register was written successfully.
 */
bool TPS65987::Controller::write_register(const reg_t reg)
{
    // Set the target address
    this->i2c->set_target_address(i2c_address);

    // Send the data buffer to the register
    this->buffer_data[0] = reg.address;
    this->buffer_data[1] = reg.length;
    return this->i2c->send_array(&this->buffer_data[0], reg.length + 2);
};

/**
 * @brief Read the currently active command and update the
 * active command property.
 * @return Returns True when the command register was read successfully.
 */
bool TPS65987::Controller::read_active_command(void)
{
    // Read the data
    if(!this->read_register(Register::Cmd1)) return false;

    // Convert the data
    this->cmd_active[3] = this->buffer_data[1];
    this->cmd_active[2] = this->buffer_data[2];
    this->cmd_active[1] = this->buffer_data[3];
    this->cmd_active[0] = this->buffer_data[4];
    return true;
};

/**
 * @brief Write a command to the command register
 * The Data1 register has to be written first, when the 
 * command expects data!
 * @param cmd The command string => has to have 4 characters.
 * @return Returns True when the command was send successfully.
 */
bool TPS65987::Controller::write_command(const char* cmd)
{
    // Assemble the cmd array
    this->buffer_cmd[0] = Register::Cmd1.address;
    this->buffer_cmd[1] = Register::Cmd1.length;
    for (unsigned char nByte=0; nByte<4; nByte++)
        this->buffer_cmd[nByte+2] = *(cmd + nByte);

    // Send the array
    unsigned char* dest = reinterpret_cast<unsigned char*>(&this->buffer_cmd[0]);
    return this->i2c->send_array(dest, Register::Cmd1.length + 2);
};
 /**
  * @brief Read the active mode of the controller.
  * @return Returns True when the mode was read successfully.
  */
 bool TPS65987::Controller::read_mode(void)
 {
     // Read the mode from the controller
    if(!this->read_register(Register::Mode)) return false;

    // Decode the mode
    switch(this->buffer_data[4])
    {
        case 'B': this->mode_active = Mode::BOOT; break;
        case 'P': this->mode_active = Mode::PTCH; break;
        case 'A': this->mode_active = Mode::APP; break;
        default:  this->mode_active = Mode::OTHER; break;
    };
    return true;
 }

 /**
  * @brief Read the current PD status from the PD Controller.
  * @return Returns True when the status was read successfully.
  */
 bool TPS65987::Controller::read_PD_status(void)
 {
     // Read the status register
     if (!this->read_register(Register::PD_Status)) return false;

     // Convert the received data
    this->i2c_data.byte[3] = this->buffer_data[1];
    this->i2c_data.byte[2] = this->buffer_data[2];
    this->i2c_data.byte[1] = this->buffer_data[3];
    this->i2c_data.byte[0] = this->buffer_data[4];

    // Check plug type
    unsigned char temp = this->i2c_data.byte[0];
    if (temp & PlugDetails_0)
        this->contract_active.USB_type = 2;
    else 
        this->contract_active.USB_type = 3;

    // Active role
    if (temp & PresentRole)
        this->contract_active.role = 1;
    else
        this->contract_active.role = 0;

    // Check CC Mode
    temp = (temp & (CCPullUp_0 | CCPullUp_1)) >> 2;
    switch (temp)
    {
        // USB Default power
        case 1: 
            this->contract_active.voltage = 5000; 
            this->contract_active.current = 900; 
            break;

        // USB power increased to 1.5A
        case 2: 
            this->contract_active.voltage = 5000; 
            this->contract_active.current = 1500;
            break;

        // USB power increased to 3.0A
        case 3: 
            this->contract_active.voltage = 5000; 
            this->contract_active.current = 3000;
            break;

        // No cable or contract
        default: 
            this->contract_active.voltage = 0; 
            this->contract_active.current = 0;
            break;
    };


     return true;
 };