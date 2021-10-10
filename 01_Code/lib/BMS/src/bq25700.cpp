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
 * @file    bq25700.cpp
 * @author  SO
 * @version v1.0.0
 * @date    14-September-2021
 * @brief   Driver for the BQ25700 battery charger controller.
 ******************************************************************************
 */

// === Includes ===
#include "bq25700.h"

// === Functions ===

/**
 * @brief Constructor for charger controller.
 * @param i2c_controller The reference to the used i2c peripheral.
 */
BQ25700::Controller::Controller(I2C::Controller_Base& i2c_controller):
i2c(&i2c_controller)
{
    // Reset i2c data buffer
    this->i2c_data.value = 0x00;
};

/**
 * @brief Read a register byte of the target and return 
 * the data sorted into the right order.
 * @return Returns True when the register was read successfully.
 */
bool BQ25700::Controller::read_register(const Register reg)
{
    // Set the target address
    this->i2c->set_target_address(i2c_address);

    // All read from the BQ25700 contain words
    if(!this->i2c->read_word(static_cast<unsigned char>(reg))) return false;

    // Read successfull, sort the data
    this->i2c_data.byte[0] = this->i2c->get_rx_data().byte[1];
    this->i2c_data.byte[1] = this->i2c->get_rx_data().byte[0];
    return true;
};

/**
 * @brief Write a register. The function automatically
 * converts from MSB-first to LSB-first.
 * @param reg The target register
 * @param data The data to be written to the register.
 * @return Returns True when the transfer was successful.
 */
bool BQ25700::Controller::write_register(const Register reg,
    const unsigned int data)
{
    // Assemble the data
    this->i2c_data.byte[2] = static_cast<unsigned char>(reg);
    this->i2c_data.byte[1] = data & 0xFF;
    this->i2c_data.byte[0] = data >> 8;
    return this->i2c->send_data(this->i2c_data, 3);
};

/**
 * @brief Initialize the charger
 * @return Returns True when the charger responds and was
 * initialized successfully.
 */
bool BQ25700::Controller::initialize(void)
{
    // check whether device is responding and the IDs match
    this->state = State::Error;
    if(this->read_register(Register::Manufacturer_ID))
    {
        if(this->i2c_data.byte[0] == manufacturer_id)
            this->state = State::Init;
        else
            this->state = State::Error;
    };

    if(this->read_register(Register::Device_ID))
    {
        if(this->i2c_data.byte[0] == device_id)
            this->state = State::Init;
        else
            this->state = State::Error;
    };
    return true;
};

/**
 * @brief Get the current state of the charger.
 * @return Returns the current state.
 */
BQ25700::State BQ25700::Controller::get_state(void) const
{
    return this->state;
};

/**
 * @brief Set the charge current and send the value to the target.
 * @param current The new charge current in [mA]
 * @return Returns True when the charge current was send successfully.
 */
bool BQ25700::Controller::set_charge_current(const unsigned int current)
{
    // Convert the current to the charger resolution
    this->current_charge = current & 0x1FC0;

    // Sent the new charger current
    return this->write_register(Register::Charge_Current, this->current_charge);
};

/**
 * @brief Set the OTG voltage and send the value to the target.
 * @param voltage The OTG voltage in mV
 * @return Returns True when the charge current was send successfully.
 */
bool BQ25700::Controller::set_OTG_voltage(const unsigned int voltage)
{
    // Convert the value to the charger resolution and clamp to min voltage
    if (voltage >= 4480)
        this->voltage_OTG = (voltage - 4480) & 0x1FC0;
    else
        this->voltage_OTG = 0;

    // Sent the new charger current
    return this->write_register(Register::OTG_Voltage, this->voltage_OTG);
};

/**
 * @brief Set the OTG current and send the value to the target.
 * @param current The OTG voltage in mA
 * @return Returns True when the charge current was send successfully.
 */
bool BQ25700::Controller::set_OTG_current(const unsigned int current)
{
    // Convert the value to the charger resolution
    this->current_OTG = (current/50) << 8;

    // Sent the new charger current
    return this->write_register(Register::OTG_Current, this->current_OTG);
};

/**
 * @brief Set the OTG voltage when no error occurred.
 * @param state Whether to turn the OTG on or off
 * @return Returns True when the action was performed without errors.
 */
bool BQ25700::Controller::enable_OTG(const bool state)
{
    if(state)
    {
        this->state = State::OTG;
        return this->write_register(Register::Charge_Option_3, EN_OTG);
    }
    else
    {
        this->state = State::Idle;
        return this->write_register(Register::Charge_Option_3, 0x00);
    }
};