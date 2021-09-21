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
 * @file    test_display.cpp
 * @author  SO
 * @version v1.0.0
 * @date    14-September-2021
 * @brief   Unit tests to test the display driver.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include "ssd1306.h"

/** === Test List ===
 * ✓ display driver initializes hardware correctly
 * ✓ display can be turned on
 * ✓ display can be tuned off
 * ▢ buffer content can be sent to display
 */

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

    // *** Constructor
    I2C_Mock() {};

    // *** overrides for interface
    void set_target_address  (const unsigned char address) override { call_set_target_address.add_call((int) address); };
    bool send_data           (const I2C::Data_t payload, const unsigned char n_bytes) override { call_send_data.add_call((int) payload.word[0]); return true; };
    bool send_byte           (const unsigned char data) override { call_send_byte.add_call((int) data); return true; };
    bool send_word           (const unsigned int data) override { call_send_word.add_call((int) data); return true; };
    bool send_array          (const unsigned char* data, const unsigned char n_bytes) override { call_send_array.add_call((int) n_bytes); return true; };
    bool send_array_leader   (const unsigned char byte, const unsigned char* data, const unsigned char n_bytes) override { call_send_array_leader.add_call((int) byte); return true; };
    bool read_data           (const unsigned char reg, unsigned char n_bytes) override { return true; };
    bool read_byte           (const unsigned char reg) override { return true; };
    bool read_word           (const unsigned char reg) override { return true; };
    bool read_array          (const unsigned char reg, unsigned char* dest, const unsigned char n_bytes) override {return true; } ;
    I2C::Data_t  get_rx_data (void) const override { I2C::Data_t data; return data; };
};

void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// === Define Tests ===
void test_init(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    SSD1306::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.initialize());
    i2c.call_set_target_address.assert_called_once_with(SSD1306::i2c_address);
    TEST_ASSERT_TRUE(i2c.call_send_data.call_count);
};

/// @brief Test whether the display can be turned on and off
void test_on_and_off(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    SSD1306::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.on());
    i2c.call_send_data.assert_called_last_with(static_cast<unsigned char>(SSD1306::Command::display_on));

    TEST_ASSERT_TRUE(UUT.off());
    i2c.call_send_data.assert_called_last_with(static_cast<unsigned char>(SSD1306::Command::display_off));
};

/// @brief Test whether a canvas buffer can be send.
void test_draw_buffer(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;
    unsigned char buffer[128*32/8];

    // create the controller object
    SSD1306::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.draw(buffer));
    TEST_ASSERT_EQUAL(4, i2c.call_send_array_leader.call_count);
    i2c.call_send_array_leader.assert_called_last_with(0x40);
};

/// === Run Tests ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_on_and_off();
    test_draw_buffer();
    UNITY_END();
    return 0;
};
