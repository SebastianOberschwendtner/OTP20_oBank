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
 * @file    ssd1306.cpp
 * @author  SO
 * @version v1.0.0
 * @date    14-September-2021
 * @brief   Driver for the SSD1306 display controller.
 ******************************************************************************
 */

// === Includes ===
#include "ssd1306.h"

// === Functions ===

/**
 * @brief Constructor for display controller.
 * @param i2c_controller The reference to the used i2c peripheral.
 */
SSD1306::Controller::Controller(I2C::Controller_Base& i2c_controller):
i2c(&i2c_controller)
{
    this->payload.value = 0x00;
};

/**
 * @brief Initialize the display controller.
 * @return Returns True when the display was initialized without any errors.
 */
bool SSD1306::Controller::initialize(void)
{
    // Set the target address for the i2c controller
    this->i2c->set_target_address(i2c_address);

    // send the initialization data
    if (!this->send_command_byte(Command::display_off))          return false;
    if (!this->send_command_byte(Command::set_disp_clock_div))   return false;
    if (!this->send_command_data(0x80))                          return false;
    if (!this->send_command_byte(Command::set_multiplex))        return false;
    if (!this->send_command_data(0x1F))                          return false;
    if (!this->send_command_byte(Command::set_display_offset))   return false;
    if (!this->send_command_data(0x00))                          return false;
    if (!this->send_command_byte(Command::set_startline))        return false;
    if (!this->send_command_byte(Command::charge_pump))          return false;
    if (!this->send_command_data(0x14))                          return false;
    if (!this->send_command_byte(Command::memory_mode))          return false;
    if (!this->send_command_data(0x00))                          return false;
    if (!this->send_command_data(0xA1))                          return false;
    if (!this->send_command_byte(Command::com_scan_dec))         return false;
    if (!this->send_command_byte(Command::set_com_pins))         return false;
    if (!this->send_command_data(0x02))                          return false;
    if (!this->send_command_byte(Command::set_contrast))         return false;
    if (!this->send_command_data(0x8F))                          return false;
    if (!this->send_command_byte(Command::set_precharge))        return false;
    if (!this->send_command_data(0xF1))                          return false;
    if (!this->send_command_byte(Command::set_vcom_detect))      return false;
    if (!this->send_command_data(0x40))                          return false;
    if (!this->send_command_byte(Command::display_ram))          return false;
    if (!this->send_command_byte(Command::display_normal))       return false;
    if (!this->send_command_byte(Command::scroll_deactivate))    return false;
    if (!this->send_command_byte(Command::column_address))       return false;
    if (!this->send_command_data(0x00))                          return false;
    if (!this->send_command_data(0x7F))                          return false;
    if (!this->send_command_byte(Command::page_address))         return false;
    if (!this->send_command_data(0x00))                          return false;
    if (!this->send_command_data(0x03))                          return false;

    return true;
};

/**
 * @brief Send a command with a single byte to the display
 * @param cmd The command byte to send.
 * @return Returns True when the command was sent successfully.
 */
bool SSD1306::Controller::send_command_byte(const Command cmd)
{
    // Set the payload, a single byte sends always two bytes:
    // The first byte (0x00) indicates that the following data
    // is a command
    this->payload.byte[1] = 0x00;
    this->payload.byte[0] = static_cast<unsigned char>(cmd);
    return this->i2c->send_data(this->payload, 2);
};

/**
 * @brief Send a command with a single data byte to the display
 * @param cmd The command byte to send.
 * @return Returns True when the command was sent successfully.
 */
bool SSD1306::Controller::send_command_data(const unsigned char cmd)
{
    // Set the payload, a single byte sends always two bytes:
    // The first byte (0x00) indicates that the following data
    // is a command
    this->payload.byte[1] = 0x00;
    this->payload.byte[0] = cmd;
    return this->i2c->send_data(this->payload, 2);
};

/**
 * @brief Turn the display on.
 * @return Returns True when the dispaly did respond correctly.
 */
bool SSD1306::Controller::on(void)
{
    return this->send_command_byte(Command::display_on);
};

/**
 * @brief Turn the display off.
 * @return Returns True when the dispaly did respond correctly.
 */
bool SSD1306::Controller::off(void)
{
    return this->send_command_byte(Command::display_off);
};

/**
 * @brief Send a buffer with the display content.
 * @param buffer Pointer to the buffer with the content.
 * @return Returns True when the buffer was sent successfully.
 */
bool SSD1306::Controller::draw(const unsigned char* buffer)
{
    // Send all 4 pages of the buffer
    for (unsigned char iPage=0; iPage<4; iPage++)
        if(!this->i2c->send_array_leader(0x40, (buffer + 128*iPage), 128))
            return false;
    
    // Sending was successfull
    return true;
};