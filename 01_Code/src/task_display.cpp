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
 * @file    task_display.cpp
 * @author  SO
 * @version v1.0.0
 * @date    14-September-2021
 * @brief   Display task of the oBank
 ******************************************************************************
 */

// === Includes ===
#include "task_display.h"

// === Global data within task ===
Graphics::Buffer_BW<128, 32> GUI_Buffer;

// === Functions ===

/**
 * @brief Main task for handling the display interface
 */
void Task_Display(void)
{
    // Setup stuff
    // *** I/O pins ***
    GPIO::PIN SCL(GPIO::PORTA, GPIO::PIN8);
    GPIO::PIN SDA(GPIO::PORTB, GPIO::PIN4);

    // *** i2c interface ***
    I2C::Controller i2c(I2C::I2C_3, 100000);
    i2c.assign_pin(SCL);
    i2c.assign_pin(SDA);
    i2c.enable();

    for(volatile int wait = 0; wait < 10000; wait++);

    // *** Display controller ***
    SSD1306::Controller Display(i2c);
    Display.initialize();

    // Create the canvas and register it for IPC
    Graphics::Canvas_BW GUI(GUI_Buffer.data.data(), GUI_Buffer.width_px, GUI_Buffer.height_px);
    GUI.fill(Graphics::Black);

    IPC::Manager ipc(IPC::Check::PID<PID::Display>());
    ipc.register_data(&GUI);
    
    // GUI.add_string("Testline");
    Display.on();

    // Start looping
    while(1)
    {
        Display.draw(&GUI_Buffer.data[0]);
        OTOS::Task::yield();
    };
};