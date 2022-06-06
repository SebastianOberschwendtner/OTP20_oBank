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
 * @version v2.0.0
 * @date    14-September-2021
 * @brief   Display task of the oBank
 ******************************************************************************
 */

// === Includes ===
#include "task_display.h"

// === Global data within task ===
// *** I/O pins ***
static GPIO::PIN SCL(GPIO::Port::A, 8);
static GPIO::PIN SDA(GPIO::Port::B, 4);
// *** i2c controller ***
static I2C::Controller i2c(IO::I2C_3, 100000);
// *** Display controller ***
static SSD1306::Controller Display(i2c);
// *** Display buffer ***
static Graphics::Buffer_BW<128, 32> GUI_Buffer;
// *** Display canvas object ***
static Graphics::Canvas_BW GUI(GUI_Buffer.data.data(), GUI_Buffer.width_px, GUI_Buffer.height_px);
// *** IPC Mananger ***
static IPC::Manager ipc_manager(IPC::Check::PID<PID::Display>());
// *** IPC Interface ***
static Display_Interface ipc_interface;
// *** IPC task references
static BMS_Interface* task_bms;
// *** String buffer ***
static char line_string[24] = {0};

// === Functions ===
static void initialize(void);
static void get_ipc(void);
static void draw_main_info(void);

/**
 * @brief Main task for handling the display interface
 */
void Task_Display(void)
{
    // Setup stuff
    initialize();

    // Get IPC references
    get_ipc();

    // Start looping
    while(1)
    {
        draw_main_info();
        Display.draw(&GUI_Buffer.data[0]);
        OTOS::Task::yield();
    };
};

/**
 * @brief Initialize the task
 */
static void initialize(void)
{
    // Register the IPC interface
    ipc_manager.register_data(&ipc_interface);

    // Set up the i2c interface
    SCL.set_alternate_function(IO::I2C_3);
    SDA.set_alternate_function(IO::I2C_3);
    i2c.enable();

    // Initialize the display
    Display.initialize();

    // Empty the canvas
    GUI.fill(Graphics::Black);

    // Write the first content in canvas    
    GUI.set_fontsize(Font::Size::Normal);
    GUI.add_string("U: \nI: ");
    Display.on();
    OTOS::Task::yield();
};

/**
 * @brief Get the IPC data needed for task execution
 */
static void get_ipc(void)
{
    // BMS Task
    while (!IPC::Manager::get_data(PID::BMS)) OTOS::Task::yield(); 
    task_bms = static_cast<BMS_Interface*>(IPC::Manager::get_data(PID::BMS).value());
};

/**
 * @brief Draw the main information on the display.
 */
static void draw_main_info(void)
{
    // Draw the voltage information
    unsigned int voltage = task_bms->get_battery_voltage();
    GUI.set_cursor(2,0);
    std::sprintf(line_string, " %d.%02d  V", voltage/1000, (voltage/10)%100);
    GUI.add_string(line_string);

    // Draw the current information
    signed int current = task_bms->get_battery_current();
    GUI.set_cursor(2,1);
    std::sprintf(line_string, "%+5d mA", current);
    GUI.add_string(line_string);
};

// === IPC interface ===

/**
 * @brief Set everything up for sleepmode
 */
void Display_Interface::sleep(void)
{
    Display.off();
};

/**
 * @brief Set everything up after waking up
 */
void Display_Interface::wake(void)
{
    Display.on();
};