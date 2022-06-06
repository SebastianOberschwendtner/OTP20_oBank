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
 * @file    task_pd.cpp
 * @author  SO
 * @version v2.0.0
 * @date    09-October-2021
 * @brief   Task for handling the Power Delivery of the oBank
 ******************************************************************************
 */

// === Includes ===
#include "task_pd.h"

// === Global data within task ===
// *** I/O pins ***
static GPIO::PIN SCL(GPIO::Port::A, 9);
static GPIO::PIN SDA(GPIO::Port::A, 10);
// *** i2c controller ***
static I2C::Controller i2c(IO::I2C_1, 100000);
// *** PD controller ***
static TPS65987::Controller PD(i2c);
// *** IPC Mananger ***
static IPC::Manager ipc_manager(IPC::Check::PID<PID::PD>());
// *** IPC Interface ***
static PD_Interface ipc_interface;

// === Functions ===
static void initialize(void);

/**
 * @brief Main task for handling the display interface
 */
void Task_PD(void)
{
    // Setup stuff
    initialize();

    // Start looping
    while(1)
    {
        OTOS::Task::yield();
    };
};

/**
 * @brief Initialize the PD task.
 */
static void initialize(void)
{
    // Register the IPC interface
    ipc_manager.register_data(&ipc_interface);

    // Set up the i2c interface
    SCL.set_alternate_function(IO::I2C_1);
    SDA.set_alternate_function(IO::I2C_1);
    i2c.enable();

    // Initialize the charger interface
    // PD.initialize();
    OTOS::Task::yield();
};

// === IPC interface ===

/**
 * @brief Set everything up for sleepmode
 */
void PD_Interface::sleep(void)
{
};

/**
 * @brief Set everything up after waking up
 */
void PD_Interface::wake(void)
{
};