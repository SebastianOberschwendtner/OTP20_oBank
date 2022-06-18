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
#include "tasks.h"
#include "gui.h"

// === Global data within task ===
// *** I/O pins ***
static GPIO::PIN SCL(GPIO::Port::A, 8);
static GPIO::PIN SDA(GPIO::Port::B, 4);
// *** i2c controller ***
static I2C::Controller i2c(IO::I2C_3, 100000);
// *** Display controller ***
static SSD1306::Controller Display(i2c);
// *** IPC Mananger ***
static IPC::Manager ipc_manager(IPC::Check::PID<IPC::Display>());
// *** IPC Interface ***
static IPC::Display_Interface ipc_interface;
// *** IPC task references
static IPC::BMS_Interface *task_bms;
// *** Current page ***
static unsigned char page = 0;

// === Functions ===
static void initialize(void);
static void get_ipc(void);

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
    while (1)
    {
        // switch page
        switch (page)
        {
        case 0:
            // *** Page 0 ***
            GUI::draw_main_info(
                task_bms->get_battery_voltage(),
                task_bms->get_battery_current());
            break;
        case 1:
            // *** Page 1 ***
            GUI::draw_state_info(false, false);
            break;
        default:
            break;
        };
        Display.draw(GUI::get_data_pointer());
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
    GUI::initialize_canvas();
    Display.initialize();

    Display.on();
    OTOS::Task::yield();
};

/**
 * @brief Get the IPC data needed for task execution
 */
static void get_ipc(void)
{
    // BMS Task
    while (!IPC::Manager::get_data(IPC::BMS))
        OTOS::Task::yield();
    task_bms = static_cast<IPC::BMS_Interface *>(IPC::Manager::get_data(IPC::BMS).value());
};

// === IPC interface ===

/**
 * @brief Set everything up for sleep mode
 */
void IPC::Display_Interface::sleep(void)
{
    ::Display.off();
};

/**
 * @brief Set everything up after waking up
 */
void IPC::Display_Interface::wake(void)
{
    ::Display.on();
};

/**
 * @brief Increase the display page
 */
void IPC::Display_Interface::next_page(void)
{
    page++;
    if (page > 1)
        page = 0;
};