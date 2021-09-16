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
 * @file    task_system.cpp
 * @author  SO
 * @version v1.0.0
 * @date    16-September-2021
 * @brief   Main system task of the oBank.
 ******************************************************************************
 */

// === Includes ===
#include "task_system.h"

// === Global data within task ===

// === Functions ===

/**
 * @brief Main task for system stuff of the oBank
 */
void Task_System(void)
{
    // Setup stuff
    // *** I/O pins ***
    GPIO::PIN Button(GPIO::PORTA, GPIO::PIN0, GPIO::INPUT);
    GPIO::PIN Led_Green(GPIO::PORTB, GPIO::PIN0, GPIO::OUTPUT);
    GPIO::PIN Led_Red(GPIO::PORTB, GPIO::PIN1, GPIO::OUTPUT);

    // *** get the GUI interface
    YIELD_WHILE(!IPC::Manager::get_data(1));
    Graphics::Canvas_BW* p_GUI = 
        reinterpret_cast<Graphics::Canvas_BW*>
        (IPC::Manager::get_data(PID::Display).value());

    // *** Create own canvas
    // p_GUI->add_string("\nThis is main!");

    GPIO::PIN SCL(GPIO::PORTA, GPIO::PIN9);
    GPIO::PIN SDA(GPIO::PORTA, GPIO::PIN10);
    I2C::Controller i2c(I2C::I2C_1, 100000);
    i2c.assign_pin(SCL);
    i2c.assign_pin(SDA);
    i2c.set_target_address(0x40);
    i2c.enable();

    p_GUI->add_string("PD Controller ");
    if (i2c.send_byte(0x00))
        p_GUI->add_string(".... ok");
    else
        p_GUI->add_string(". fault");

    i2c.set_target_address(0x12);
    p_GUI->add_string("\nCharger ");
    if (i2c.send_byte(0x00))
        p_GUI->add_string(".......... ok");
    else
        p_GUI->add_string("....... fault");


    i2c.set_target_address(0x20);
    p_GUI->add_string("\nBalancer ");
    if (i2c.send_byte(0x00))
        p_GUI->add_string("......... ok");
    else
    {
        char err_msg[5] = {0};
        std::sprintf(&err_msg[0], "%-03d", i2c.get_error()); 
        p_GUI->add_string("....... ");
        p_GUI->add_string(err_msg);
    };

    // Start looping
    while(1)
    {
        Led_Red.set(Button.get());
        OTOS::Task::yield();
    };
};