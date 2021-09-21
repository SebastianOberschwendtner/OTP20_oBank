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
 * @file    test_bq25700.cpp
 * @author  SO
 * @version v1.0.0
 * @date    17-September-2021
 * @brief   Unit tests to test the driver for battery charger.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include "bq25700.h"

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
        if (reg == 0xFE) this->rx_data.value = 0x4000;
        if (reg == 0xFF) this->rx_data.value = 0x7900;
        return true; };
    bool read_array          (const unsigned char reg, unsigned char* dest, const unsigned char n_bytes) override {return true; } ;
    I2C::Data_t  get_rx_data (void) const override { return this->rx_data; };
};

/** === Test List ===
 * ▢ controller has the properties:
 *      ✓ measured system voltage in xx.x V
 *      ✓ measured input voltage in xx.x V
 *      ✓ measured input current (+/-) in x.xxx A
 *      ✓ OTG voltage (setpoint) in xx.x V
 *      ✓ OTG current (setpoint) in x.xxx A
 *      ✓ Charge current (setpoint) in x.xxx A
 * ✓ Controller checks whether target is responding during construction of the controller.
 * ▢ Controller has states:
 *      ▢ Init
 *      ▢ Idle
 *      ▢ Charging
 *      ▢ OTG
 *      ▢ Error
 * ▢ controller can be assigned to the pins:
 *      ▢ EN_OTG (output)
 *      ▢ CHRG_OK (input)
 *      ▢ PROCHOT (input)
 * ▢ controller can set options:
 *      ▢ options registers
 *      ▢ Charge current
 *      ▢ OTG current
 *      ▢ OTG voltage
 * ▢ Controller can read the battery status
 *      ▢ whether it is charging
 *      ▢ whether OTG is enabled
 *      ▢ faults occurred
 * ▢ controller can read the adc values
 * ▢ controller can enable
 *      ▢ battery charging
 *      ▢ OTG
 *      ▢ Sleep mode
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
    BQ25700::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_EQUAL(0, UUT.get_system_voltage());
    TEST_ASSERT_EQUAL(0, UUT.get_input_voltage());
    TEST_ASSERT_EQUAL(0, UUT.get_input_current());
    TEST_ASSERT_EQUAL(4480, UUT.get_OTG_voltage()); // OTG reset voltage is 4.480 V
    TEST_ASSERT_EQUAL(0, UUT.get_OTG_current());
    TEST_ASSERT_EQUAL(0, UUT.get_charge_current());
    i2c.call_set_target_address.assert_called_last_with(0x12);
    i2c.call_read_word.assert_called_last_with(0xFF);
    TEST_ASSERT_EQUAL(2, i2c.call_read_word.call_count);
    TEST_ASSERT_EQUAL(0, static_cast<unsigned char>(UUT.get_state()));
};

/// @brief Test sending options
void test_set_options(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    BQ25700::Controller UUT(i2c);

    // Test writting a register
    TEST_ASSERT_TRUE(UUT.write_register(BQ25700::Register::Charge_Option_0, 0x1234));
    i2c.call_send_data.assert_called_once_with(0x123412); // BQ25700 expects MSB last

    // Test setting the charge current
    TEST_ASSERT_TRUE(UUT.set_charge_current(1000));
    TEST_ASSERT_EQUAL(960U, UUT.get_charge_current());
    i2c.call_send_data.assert_called_once_with(0x14C003); // BQ25700 expects MSB last

    // Test setting the OTG voltage
    TEST_ASSERT_TRUE(UUT.set_OTG_voltage(5000));
    TEST_ASSERT_EQUAL(4992U, UUT.get_OTG_voltage());
    i2c.call_send_data.assert_called_once_with(0x3B0002); // BQ25700 expects MSB last

    // Test setting the OTG current
    TEST_ASSERT_TRUE(UUT.set_OTG_current(3300));
    TEST_ASSERT_EQUAL(0x4200U, UUT.get_OTG_current());
    i2c.call_send_data.assert_called_once_with(0x3C0042); // BQ25700 expects MSB last

    // Test enabling the OTG voltage
    TEST_ASSERT_TRUE(UUT.enable_OTG(true));
    TEST_ASSERT_EQUAL(3, static_cast<unsigned char>(UUT.get_state()));
    i2c.call_send_data.assert_called_once_with(0x320010); // BQ25700 expects MSB last
    // Test disabling the OTG voltage again
    TEST_ASSERT_TRUE(UUT.enable_OTG(false));
    TEST_ASSERT_EQUAL(1, static_cast<unsigned char>(UUT.get_state()));
    i2c.call_send_data.assert_called_once_with(0x320000); // BQ25700 expects MSB last
};

/// === Run Tests ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_set_options();
    UNITY_END();
    return 0;
};
