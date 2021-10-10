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
 * @file    task_bms.cpp
 * @author  SO
 * @version v1.0.0
 * @date    09-October-2021
 * @brief   Task for the Battery Management of the oBank
 ******************************************************************************
 */

// === Includes ===
#include "task_bms.h"

// === Global data within task ===
// *** I/O pins ***
static GPIO::PIN CHR_OK(GPIO::PORTA, GPIO::PIN2, GPIO::INPUT);
static GPIO::PIN SCL(GPIO::PORTA, GPIO::PIN9);
static GPIO::PIN SDA(GPIO::PORTA, GPIO::PIN10);
// *** i2c controller ***
static I2C::Controller i2c(I2C::I2C_1, 100000);
// *** Charger controller ***
static BQ25700::Controller Charger(i2c);
// *** BMS chip ***
static MAX17205::Controller BMS(i2c);
// *** IPC Mananger ***
static IPC::Manager ipc_manager(IPC::Check::PID<PID::BMS>());
// *** IPC Interface ***
static BMS_Interface ipc_interface;

// === Functions ===
static void initialize(void);

/**
 * @brief Main task for handling the BMS
 */
void Task_BMS(void)
{
    // Setup stuff
    initialize();

    // Start looping
    while(1)
    {
        // Read battery data
        BMS.read_battery_current_avg();
        BMS.read_battery_voltage();

        // Manage charging
        if (CHR_OK.get())
        {
            // Input power is present
            // Charger.set_charge_current(500);
        }

        OTOS::Task::yield();
    };
};

/**
 * @brief Initialize the BMS task.
 */
static void initialize(void)
{
    // Register the IPC interface
    ipc_manager.register_data(&ipc_interface);

    // Set up the i2c interface
    i2c.assign_pin(SCL);
    i2c.assign_pin(SDA);
    i2c.enable();

    // Initialize the charger interface
    CHR_OK.setPull(GPIO::PULL_UP);
    CHR_OK.enable_interrupt(GPIO::Edge::Rising);
    Charger.initialize();

    // Initialize the BMS Chip
    BMS.initialize();
    OTOS::Task::yield();
};

extern "C" void EXTI2_3_IRQHandler(void)
{
    CHR_OK.reset_pending_interrupt();
};

// === IPC interface ===

/**
 * @brief Set everything up for sleepmode
 */
void BMS_Interface::sleep(void)
{
    // Disable the pullup to save power
    CHR_OK.setPull(GPIO::NO_PP);
};

/**
 * @brief Set everything up after waking up
 */
void BMS_Interface::wake(void)
{
    // Enable the pullup to sense whether an input is present
    CHR_OK.setPull(GPIO::PULL_UP);
};

/**
 * @brief Get the latest voltage measurement of the battery
 * @return The battery voltage in [mV]
 */
unsigned int BMS_Interface::get_battery_voltage(void) const
{
    return BMS.get_battery_voltage();
};

/**
 * @brief Get the latest current measurement of the battery
 * @return The battery current in [mA]
 */
signed int BMS_Interface::get_battery_current(void) const
{
    return BMS.get_battery_current();
};

/**
 * @brief Get the current state of the charger.
 * @return The state of the charger. (Init, Idle, Charging, OTG, Error)
 */
BQ25700::State BMS_Interface::get_charger_state(void) const
{
    return Charger.get_state();
};