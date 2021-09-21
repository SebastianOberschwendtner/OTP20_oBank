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
 * @file    max17205.cpp
 * @author  SO
 * @version v1.0.0
 * @date    20-September-2021
 * @brief   Driver for the MAX17205+ battery balancer and coulomb counter.
 ******************************************************************************
 */

// === Includes ===
#include "max17205.h"

// === Functions ===

/**
 * @brief Constructor for balancer controller.
 * @param i2c_controller The reference to the used i2c peripheral.
 */
MAX17205::Controller::Controller(I2C::Controller_Base& i2c_controller):
i2c(&i2c_controller)
{
    
};

/**
 * @brief Initialize the balancer for the application.
 * @return Returns True when the balancer was initialized correctly.
 */
bool MAX17205::Controller::initialize(void)
{
    // Get the current configuration
    if(!this->read_register(Register::PackCfg)) return false;

    // Check the configuration
    /// @todo Should the configuration just be written every time and not checked?
    unsigned int config = this->i2c_data.word[0];
    if(! (config & ChEn))
        return this->write_register(Register::PackCfg, (TdEn | ChEn | BALCFG_1 | BALCFG_0 | 2U));

    return true;
};

/**
 * @brief Convert a raw voltage value to a mV value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
inline unsigned int MAX17205::Controller::to_voltage( const unsigned int raw )
{
    // voltage = (10 * raw / 128) mV
    return (10 * raw) >> 7;
};

/**
 * @brief Convert a raw current to a mA value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
inline signed int MAX17205::Controller::to_current( const unsigned int raw )
{
    // current = ( raw / (640*R_sense) ) mA

    // convert to signed number
    signed long temp = 0;
    if (raw & (1<<15))
        temp = raw - 65536;
    else
        temp = raw;

    // Convert the raw data
    return (1000 * temp) / (640 * R_sense_mOhm);
};

/**
 * @brief Convert a raw capacity value to a mAh value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
inline unsigned int MAX17205::Controller::to_capacity( const unsigned int raw )
{
    // Capacity = ( raw * 0.005 / R_sense) mAh
    return raw * (5 / R_sense_mOhm);
};

/**
 * @brief Convert a raw percentage value to a % value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
inline unsigned int MAX17205::Controller::to_percentage( const unsigned int raw )
{
    // percentage = raw / 256
    return raw >> 8;
};

/**
 * @brief Convert a raw temperature value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
inline signed int MAX17205::Controller::to_temperature( const unsigned int raw )
{
    // temperature = raw / 256
    return raw / 256;
};

/**
 * @brief Convert a raw resistance value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
inline unsigned int MAX17205::Controller::to_resistance( const unsigned int raw )
{
    // resistance = raw / 4096
    return raw >> 12;
};

/**
 * @brief Get the voltage of the battery pack.
 * @return Returns the voltage in [mV].
 */
unsigned int MAX17205::Controller::get_battery_voltage(void) const
{
    return this->voltage_battery;    
};

/**
 * @brief Get the current of the battery pack.
 * @return Returns the current in [mA].
 */
signed int MAX17205::Controller::get_battery_current(void) const
{
    return this->current_battery;    
};

/**
 * @brief Get the voltage of a specific cell.
 * @param cell The number of the cell (1 or 2)
 * @return Returns the voltage in [mV].
 */
unsigned int MAX17205::Controller::get_cell_voltage(const unsigned char cell) const
{
    return this->voltage_cell[cell - 1];    
};

/**
 * @brief Get the total capacity of the battery pack.
 * @return Returns the capacity in [mAh].
 */
unsigned int MAX17205::Controller::get_total_capacity(void) const
{
    return this->capacity[0];    
};

/**
 * @brief Get the remaining capacity of the battery pack.
 * @return Returns the capacity in [mAh].
 */
unsigned int MAX17205::Controller::get_remaining_capacity(void) const
{
    return this->capacity[1];    
};

/**
 * @brief Get the number of cycles of the battery pack.
 * @return Returns the number of cycles.
 */
unsigned int MAX17205::Controller::get_number_cycles(void) const
{
    return this->cycles;    
};

/**
 * @brief Get the age of the battery pack.
 * @return Returns the age as a percentage of the original capacity.
 */
unsigned int MAX17205::Controller::get_age(void) const
{
    return this->age;    
};

/**
 * @brief Get the internal series resistance of the battery pack.
 * @return Returns the resistance in [mΩ].
 */
unsigned int MAX17205::Controller::get_ESR(void) const
{
    return this->esr;    
};

/**
 * @brief Get the temperature of the battery pack.
 * @return Returns the voltage in [mV].
 */
signed int MAX17205::Controller::get_temperature(void) const
{
    return this->temperature;    
};

/**
 * @brief Read a register of the balancer.
 * The balancer sends LSB first. The data is stored already
 * sorted to LSB last in the i2c_data struct.
 * @param reg The register to read from.
 * @return Returns True when the register was read successfully.
 */
bool MAX17205::Controller::read_register(const Register reg)
{
    unsigned int reg_int = static_cast<unsigned int>(reg);

    // Set the i2c address
    if (reg_int > 0xFF)
        this->i2c->set_target_address(i2c_address_high);
    else
        this->i2c->set_target_address(i2c_address_low);

    // perform the read
    unsigned char reg_byte = static_cast<unsigned char>(reg_int & 0xFF);
    if (!this->i2c->read_word(reg_byte)) return false;

    // Read successfull, sort the data
    this->i2c_data.byte[0] = this->i2c->get_rx_data().byte[1];
    this->i2c_data.byte[1] = this->i2c->get_rx_data().byte[0];
    return true;
};

/**
 * @brief Write a register of the balancer.
 * The balancer expects LSB first.
 * @param reg The register to write to.
 * @param data The data for the register
 * @return Returns True when the data was sent successfully.
 */
bool MAX17205::Controller::write_register(const Register reg,
    const unsigned int data)
{
    unsigned int reg_int = static_cast<unsigned int>(reg);

    // Set the i2c address
    if (reg_int > 0xFF)
        this->i2c->set_target_address(i2c_address_high);
    else
        this->i2c->set_target_address(i2c_address_low);

    // configure the payload to send LSB first
    this->i2c_data.byte[3] = 0;
    this->i2c_data.byte[2] = (reg_int & 0xFF);
    this->i2c_data.byte[1] = (data & 0xFF);
    this->i2c_data.byte[0] = (data >> 8);

    // send the data
    return this->i2c->send_data(this->i2c_data, 3);
};

/**
 * @brief Read the pack voltage from the balancer and convert to mV.
 * @return Returns True when the pack voltage was read successfully.
 */
bool MAX17205::Controller::read_battery_voltage(void)
{
    // Read the Batt Register
    if(!this->read_register(Register::Batt_Register)) return false;

    // convert the data
    unsigned long temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    temp *= 10;
    this->voltage_battery = (temp >> 3);
    return true;
};

/**
 * @brief Read the pack current from the balancer and convert to mA.
 * @return Returns True when the pack current was read successfully.
 */
bool MAX17205::Controller::read_battery_current(void)
{
    // Read the Current Register
    if(!this->read_register(Register::Current)) return false;

    // convert the data
    unsigned int temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->current_battery = this->to_current(temp);
    return true;
};

/**
 * @brief Read the average pack current from the balancer and convert to mA.
 * @return Returns True when the pack current was read successfully.
 */
bool MAX17205::Controller::read_battery_current_avg(void)
{
    // Read the Current Register
    if(!this->read_register(Register::Avg_Current)) return false;

    // convert the data
    unsigned int temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->current_battery = this->to_current(temp);
    return true;
};

/**
 * @brief Read the cell voltages of the balancer
 * @return Returns true when the cell voltages are updated successfully.
 */
bool MAX17205::Controller::read_cell_voltage(void)
{
    // Read the raw values
    unsigned char reg = static_cast<unsigned char>(Register::Cell_2);
    if(!this->i2c->read_array(reg, &this->i2c_data.byte[0], 4)) return false;

    // Convert the data
    // Cell 2
    unsigned int temp = this->i2c_data.byte[3];
    temp += this->i2c_data.byte[2] << 8;
    this->voltage_cell[0] = this->to_voltage(temp);
    // Cell 2
    temp = this->i2c_data.byte[1];
    temp += this->i2c_data.byte[0] << 8;
    this->voltage_cell[1] = this->to_voltage(temp);

    return true;
};

/**
 * @brief Read the avg cell voltages of the balancer
 * @return Returns true when the cell voltages are updated successfully.
 */
bool MAX17205::Controller::read_cell_voltage_avg(void)
{
    // Read the raw values
    unsigned char reg = static_cast<unsigned char>(Register::Avg_Cell_2);
    if(!this->i2c->read_array(reg, &this->i2c_data.byte[0], 4)) return false;

    // Convert the data
    // Cell 2
    unsigned int temp = this->i2c_data.byte[3];
    temp += this->i2c_data.byte[2] << 8;
    this->voltage_cell[0] = this->to_voltage(temp);
    // Cell 2
    temp = this->i2c_data.byte[1];
    temp += this->i2c_data.byte[0] << 8;
    this->voltage_cell[1] = this->to_voltage(temp);

    return true;
};