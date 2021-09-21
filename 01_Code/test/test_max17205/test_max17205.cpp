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
 * @file    test_max17205.cpp
 * @author  SO
 * @version v1.0.0
 * @date    20-September-2021
 * @brief   Unit tests to test the driver for battery balancer and coulomb counter.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include "max17205.h"

// === Fixtures ===

// Mock the i2c driver
class I2C_Mock: public I2C::Controller_Base, public Mock::Peripheral
{

public:
    // *** Variables to track calls
    Mock::Callable call_set_target_address;
    Mock::Callable call_send_data;
    Mock::Callable call_send_byte;
    Mock::Callable call_send_word;
    Mock::Callable call_send_array;
    Mock::Callable call_send_array_leader;
    Mock::Callable call_read_word;
    Mock::Callable call_read_array;
    I2C::Data_t rx_data;

    // *** Constructor
    I2C_Mock() {rx_data.value = 0; };

    // *** overrides for interface
    void set_target_address  (const unsigned char address) override { call_set_target_address.add_call((int) address); };
    bool send_data           (const I2C::Data_t payload, const unsigned char n_bytes) override { call_send_data.add_call((int) payload.word[0]); return true; };
    bool send_byte           (const unsigned char data) override { call_send_byte.add_call((int) data); return true; };
    bool send_word           (const unsigned int data) override { call_send_word.add_call((int) data); return true; };
    bool send_array          (const unsigned char* data, const unsigned char n_bytes) override { call_send_array.add_call((int) n_bytes); return true; };
    bool send_array_leader   (const unsigned char byte, const unsigned char* data, const unsigned char n_bytes) override { call_send_array_leader.add_call((int) byte); return true; };
    bool read_data           (const unsigned char reg, unsigned char n_bytes) override { return true; };
    bool read_byte           (const unsigned char reg) override { return true; };
    bool read_word           (const unsigned char reg) override { 
        call_read_word.add_call((int)reg);
        return true; };
    bool read_array          (const unsigned char reg, unsigned char* dest, const unsigned char n_bytes) override {
        call_read_array.add_call((int) reg);
        for (unsigned char nByte=0; nByte<n_bytes; nByte++) *(dest + nByte) = this->rx_data.byte[nByte];
        return true; } ;
    I2C::Data_t  get_rx_data (void) const override { return this->rx_data; };
};

/** === Test List ===
 * ✓ controller has properties:
 *      ✓ Battery voltage in mV
 *      ✓ Battery current in mA
 *      ✓ Cell 1 voltage in mA
 *      ✓ Cell 2 voltage in mA
 *      ✓ Total battery capacity in mAh
 *      ✓ Remaining battery capacity in mAh
 *      ✓ Number of cycles
 *      ✓ Age of battery
 *      ✓ Series Resistance of battery in mΩ
 *      ✓ Temperature
 * ✓ controller can write registers
 * ✓ controller can read registers
 * ✓ controller can set correct initialization data
 * ▢ controller read and convert correctly:
 *      ▢ Capacity
 *      ▢ Percentage
 *      ✓ Voltage
 *      ✓ Cell Voltage
 *      ▢ Average Cell Voltage
 *      ✓ Current
 *      ✓ Average Current
 *      ▢ Temperature
 *      ▢ Resistance
 *      ▢ Time
 * ▢ controller can enable/disable balancing
 * ▢ controller can enable/disable sleep mode
 */

void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// === Define Tests ===
/// @brief Test the constructor
void test_init(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // Test the properties
    TEST_ASSERT_EQUAL(0, UUT.get_battery_voltage());
    TEST_ASSERT_EQUAL(0, UUT.get_battery_current());
    TEST_ASSERT_EQUAL(0, UUT.get_cell_voltage(1));
    TEST_ASSERT_EQUAL(0, UUT.get_cell_voltage(2));
    TEST_ASSERT_EQUAL(0, UUT.get_total_capacity());
    TEST_ASSERT_EQUAL(0, UUT.get_remaining_capacity());
    TEST_ASSERT_EQUAL(0, UUT.get_number_cycles());
    TEST_ASSERT_EQUAL(0, UUT.get_age());
    TEST_ASSERT_EQUAL(0, UUT.get_ESR());
    TEST_ASSERT_EQUAL(0, UUT.get_temperature());
    
    // initialization
    TEST_ASSERT_TRUE(UUT.initialize());
    i2c.call_read_word.assert_called_once_with(0xBD);
};

/// @brief Test writting of registers
void test_write_register(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    // send low address
    TEST_ASSERT_TRUE(UUT.write_register(MAX17205::Register::Cell_1, 0x1234));
    i2c.call_set_target_address.assert_called_once_with(MAX17205::i2c_address_low);
    i2c.call_send_data.assert_called_once_with(0xD83412);

    // send high address
    TEST_ASSERT_TRUE(UUT.write_register(MAX17205::Register::nConfig, 0x1234));
    i2c.call_set_target_address.assert_called_once_with(MAX17205::i2c_address_high);
    i2c.call_send_data.assert_called_once_with(0xB03412);
};

/// @brief Test reading of registers
void test_read_register(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    // send low address
    TEST_ASSERT_TRUE(UUT.read_register(MAX17205::Register::Cell_1));
    i2c.call_set_target_address.assert_called_once_with(MAX17205::i2c_address_low);
    i2c.call_read_word.assert_called_once_with(0xD8);

    // send high address
    TEST_ASSERT_TRUE(UUT.read_register(MAX17205::Register::nConfig));
    i2c.call_set_target_address.assert_called_once_with(MAX17205::i2c_address_high);
    i2c.call_read_word.assert_called_once_with(0xB0);
};

/// @brief test reading the battery voltage
void test_read_battery_voltage(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    i2c.rx_data.word[1] = 0x00;
    i2c.rx_data.word[0] = 0xA00F;
    TEST_ASSERT_TRUE(UUT.read_battery_voltage());
    TEST_ASSERT_EQUAL(5000, UUT.get_battery_voltage());
    i2c.call_read_word.assert_called_once_with(0xDA);

    i2c.rx_data.word[1] = 0;
    i2c.rx_data.word[0] = 0xC012;
    TEST_ASSERT_TRUE(UUT.read_battery_voltage());
    TEST_ASSERT_EQUAL(6000, UUT.get_battery_voltage());
};

/// @brief test reading the battery current
void test_read_battery_current(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    i2c.rx_data.word[0] = 0x800C;
    TEST_ASSERT_TRUE(UUT.read_battery_current());
    TEST_ASSERT_EQUAL(1000, UUT.get_battery_current());
    i2c.call_read_word.assert_called_once_with(0x0A);

    i2c.rx_data.word[0] = 0x4006;
    TEST_ASSERT_TRUE(UUT.read_battery_current());
    TEST_ASSERT_EQUAL(500, UUT.get_battery_current());
};

/// @brief test reading the cell voltage
void test_read_cell_voltage(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    i2c.rx_data.byte[3] = 0x00;
    i2c.rx_data.byte[2] = 0xA5;
    i2c.rx_data.byte[1] = 0x00;
    i2c.rx_data.byte[0] = 0xD2;
    TEST_ASSERT_TRUE(UUT.read_cell_voltage());
    TEST_ASSERT_EQUAL( 3300, UUT.get_cell_voltage(1));
    TEST_ASSERT_EQUAL( 4200, UUT.get_cell_voltage(2));
    i2c.call_read_array.assert_called_once_with(0xD7);
};

/// @brief test reading the battery current
void test_read_battery_current_avg(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    i2c.rx_data.word[0] = 0x800C;
    TEST_ASSERT_TRUE(UUT.read_battery_current_avg());
    TEST_ASSERT_EQUAL(1000, UUT.get_battery_current());
    i2c.call_read_word.assert_called_once_with(0x0B);

    i2c.rx_data.word[0] = 0x4006;
    TEST_ASSERT_TRUE(UUT.read_battery_current_avg());
    TEST_ASSERT_EQUAL(500, UUT.get_battery_current());
};

/// @brief test reading the cell voltage
void test_read_cell_voltage_avg(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    i2c.rx_data.byte[3] = 0x00;
    i2c.rx_data.byte[2] = 0xA5;
    i2c.rx_data.byte[1] = 0x00;
    i2c.rx_data.byte[0] = 0xD2;
    TEST_ASSERT_TRUE(UUT.read_cell_voltage_avg());
    TEST_ASSERT_EQUAL( 3300, UUT.get_cell_voltage(1));
    TEST_ASSERT_EQUAL( 4200, UUT.get_cell_voltage(2));
    i2c.call_read_array.assert_called_once_with(0xD3);
};

/// === Run Tests ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_write_register();
    test_read_register();
    test_read_battery_voltage();
    test_read_battery_current();
    test_read_cell_voltage();
    test_read_battery_current_avg();
    test_read_cell_voltage_avg();
    UNITY_END();
    return 0;
};
