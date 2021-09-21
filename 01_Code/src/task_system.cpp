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
    GPIO::PIN EN_OTG(GPIO::PORTB, GPIO::PIN5, GPIO::OUTPUT);
    GPIO::PIN EN_5V_USB(GPIO::PORTB, GPIO::PIN6, GPIO::OUTPUT);
    GPIO::PIN CHRG_OK(GPIO::PORTA, GPIO::PIN2, GPIO::INPUT);
    CHRG_OK.setPull(GPIO::PULL_UP);

    // *** get the GUI interface
    YIELD_WHILE(!IPC::Manager::get_data(PID::Display));
    Graphics::Canvas_BW* p_GUI = 
        reinterpret_cast<Graphics::Canvas_BW*>
        (IPC::Manager::get_data(PID::Display).value());

    // *** Create the main i2c controller ***
    GPIO::PIN SCL(GPIO::PORTA, GPIO::PIN9, GPIO::OUTPUT);
    GPIO::PIN SDA(GPIO::PORTA, GPIO::PIN10, GPIO::OUTPUT);
    I2C::Controller i2c(I2C::I2C_1, 100000);
    i2c.assign_pin(SCL);
    i2c.assign_pin(SDA);
    i2c.enable();
    i2c.set_target_address(0x40);
    p_GUI->add_string("PD Controller");

    // *** Create the controller for the balancer **
    MAX17205::Controller Balancer(i2c);

    // *** Create the controller for the PD IC ***
    TPS65987::Controller PD(i2c);
    if(!PD.initialize())
        i2c.generate_stop();
    OTOS::Task::yield();

    PD.read_mode();
    p_GUI->add_char(
        static_cast<unsigned char>(PD.get_mode()) + 48
    );

    OTOS::Task::yield();
    Balancer.initialize(); 

    OTOS::Task::yield();
    char line_string[24] = {0};

    // Start looping
    while(1)
    {
        Button.read_edge();
        if(Button.rising_edge()) Led_Red.toggle();
        EN_5V_USB.set(Button.get());
        Led_Green.set(CHRG_OK.get());

        Balancer.read_battery_voltage();
        Balancer.read_battery_current();
        Balancer.read_cell_voltage();
        Balancer.read_register(MAX17205::Register::Cell_3);

        p_GUI->set_cursor(0, 1);
        std::sprintf(line_string, "Voltage: %04d mV", Balancer.get_battery_voltage());
        p_GUI->add_string(line_string);

        p_GUI->set_cursor(0, 2);
        std::sprintf(line_string, "Current: %04d mA", Balancer.get_battery_current());
        p_GUI->add_string(line_string);

        p_GUI->set_cursor(0, 3);
        std::sprintf(line_string, "U1: %04d mV", Balancer.get_cell_voltage(1));
        p_GUI->add_string(line_string);
        std::sprintf(line_string, " U2: %04d mV", Balancer.get_cell_voltage(2));
        p_GUI->add_string(line_string);

        OTOS::Task::yield();
    };
};