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
 * @file    test_tps65987.cpp
 * @author  SO
 * @version v1.0.0
 * @date    20-September-2021
 * @brief   Unit tests to test the driver for USB-3 PD controller.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include "tps65987.h"

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
    bool send_array          (const unsigned char* data, const unsigned char n_bytes) override { 
        call_send_array.add_call((int) n_bytes); 
        for (unsigned char nByte=0; nByte<4; nByte++) this->rx_data.byte[nByte] = *(data + nByte + 2);
        return true; };
    bool send_array_leader   (const unsigned char byte, const unsigned char* data, const unsigned char n_bytes) override { call_send_array_leader.add_call((int) byte); return true; };
    bool read_data           (const unsigned char reg, unsigned char n_bytes) override { return true; };
    bool read_byte           (const unsigned char reg) override { return true; };
    bool read_word           (const unsigned char reg) override { 
        call_read_word.add_call((int)reg);
        if (reg == 0xFE) this->rx_data.value = 0x4000;
        if (reg == 0xFF) this->rx_data.value = 0x7900;
        return true; };
    bool read_array          (const unsigned char reg, unsigned char* dest, const unsigned char n_bytes) override {
        call_read_array.add_call((int) n_bytes);
        for (unsigned char nByte=0; nByte<4; nByte++) *(dest+nByte) = this->rx_data.byte[nByte];
        *(dest + 4) = n_bytes - 1;
        return true; } ;
    I2C::Data_t  get_rx_data (void) const override { return this->rx_data; };
};

/** === Test List ===
 * ▢ Controller has properties:
 *      ✓ current operating mode
 *      ✓ active command
 *      ✓ Current power contract:
 *          ✓ Sink/Source
 *          ✓ USB type (1,2,3)
 *          ✓ Voltage
 *          ✓ Current
 * ✓ controller can read registers with variable length
 * ✓ controller can write registers with variable length
 * ✓ controller can send commands
 * ✓ controller can read the mode the PD IC is in
 * ✓ controller initializes the PD IC based on its mode
 * ▢ controller can check whether command was finished
 * ▢ controller can read the return code of a finished command
 * ▢ controller can read the PD status
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
    TPS65987::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_EQUAL(0, static_cast<unsigned char>(UUT.get_mode()));
    char Expected_Command[] = {0,0,0,0};
    TEST_ASSERT_EQUAL_CHAR_ARRAY(Expected_Command, UUT.get_active_command(), 4);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);
};

/// @brief Test reading a register with variable length
void test_read_register(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.read_register(TPS65987::Register::Data1));
    i2c.call_set_target_address.assert_called_once_with(TPS65987::i2c_address);
    i2c.call_read_array.assert_called_once_with(65);

    TEST_ASSERT_TRUE(UUT.read_register(TPS65987::Register::Cmd1));
    i2c.call_read_array.assert_called_once_with(5);
};

/// @brief Test writing a register with variable length
void test_write_register(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.write_register(TPS65987::Register::Data1));
    i2c.call_set_target_address.assert_called_once_with(TPS65987::i2c_address);
    i2c.call_send_array.assert_called_once_with(66);

    TEST_ASSERT_TRUE(UUT.write_register(TPS65987::Register::Cmd1));
    i2c.call_send_array.assert_called_once_with(6);
};

/// @brief Test reading a command status
void test_read_active_command(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    i2c.rx_data.byte[0] = '!';
    i2c.rx_data.byte[1] = 'D';
    i2c.rx_data.byte[2] = 'M';
    i2c.rx_data.byte[3] = 'C';
    TEST_ASSERT_TRUE(UUT.read_active_command());
    i2c.call_read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("CMD!", UUT.get_active_command(), 4);
};

/// @brief Test writting a command
void test_write_command(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.write_command("PTCc"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("PTCc", i2c.rx_data.byte, 4);
};

/// @brief Test reading the current mode of the controller
void test_read_mode(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    i2c.rx_data.byte[0] = 'H';
    i2c.rx_data.byte[1] = 'C';
    i2c.rx_data.byte[2] = 'T';
    i2c.rx_data.byte[3] = 'P';
    TEST_ASSERT_TRUE(UUT.read_mode());
    i2c.call_read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL(1, static_cast<unsigned char>(UUT.get_mode()));
};

/// @brief Test the correct initialization of the controller
void test_initialization(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    // when controller is in patch mode -> exit by sending "PTCc" command
    i2c.rx_data.byte[0] = 'H';
    i2c.rx_data.byte[1] = 'C';
    i2c.rx_data.byte[2] = 'T';
    i2c.rx_data.byte[3] = 'P';
    TEST_ASSERT_TRUE(UUT.initialize());
    i2c.call_read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("PTCc", i2c.rx_data.byte, 4);
};

/// @brief Test the reading of the PD status
void test_read_PD_status(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // Response with all zeros
    i2c.rx_data.byte[0] = 0x00;
    i2c.rx_data.byte[1] = 0x00;
    i2c.rx_data.byte[2] = 0x00;
    i2c.rx_data.byte[3] = 0x00;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    i2c.call_set_target_address.assert_called_once_with(TPS65987::i2c_address);
    i2c.call_read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);

    // Plug type is USB2.0
    i2c.rx_data.byte[0] = 0x00;
    i2c.rx_data.byte[1] = 0x00;
    i2c.rx_data.byte[2] = 0x00;
    i2c.rx_data.byte[3] = TPS65987::PlugDetails_0;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(2, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);

    // USB default current
    i2c.rx_data.byte[0] = 0x00;
    i2c.rx_data.byte[1] = 0x00;
    i2c.rx_data.byte[2] = 0x00;
    i2c.rx_data.byte[3] = TPS65987::CCPullUp_0;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(5000, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(900, UUT.get_active_contract().current);

    // USB 1.5A
    i2c.rx_data.byte[0] = 0x00;
    i2c.rx_data.byte[1] = 0x00;
    i2c.rx_data.byte[2] = 0x00;
    i2c.rx_data.byte[3] = TPS65987::CCPullUp_1;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(5000, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(1500, UUT.get_active_contract().current);

    // USB 3.0A
    i2c.rx_data.byte[0] = 0x00;
    i2c.rx_data.byte[1] = 0x00;
    i2c.rx_data.byte[2] = 0x00;
    i2c.rx_data.byte[3] = TPS65987::CCPullUp_1 | TPS65987::CCPullUp_0;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(5000, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(3000, UUT.get_active_contract().current);

    // Role source
    i2c.rx_data.byte[0] = 0x00;
    i2c.rx_data.byte[1] = 0x00;
    i2c.rx_data.byte[2] = 0x00;
    i2c.rx_data.byte[3] = TPS65987::PresentRole;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(1, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);
};

/// === Run Tests ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_read_register();
    test_write_register();
    test_read_active_command();
    test_write_command();
    test_read_mode();
    test_initialization();
    test_read_mode();
    test_read_PD_status();
    UNITY_END();
    return 0;
};
