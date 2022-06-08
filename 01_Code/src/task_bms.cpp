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
 * @version v2.0.0
 * @date    09-October-2021
 * @brief   Task for the Battery Management of the oBank
 ******************************************************************************
 */

// === Includes ===
#include "tasks.h"

// === Global data within task ===
// *** I/O pins ***
static GPIO::PIN CHR_OK(GPIO::Port::A, 2, GPIO::Mode::Input);
static GPIO::PIN SCL(GPIO::Port::A, 9);
static GPIO::PIN SDA(GPIO::Port::A, 10);
// *** i2c controller ***
static I2C::Controller i2c(IO::I2C_1, 100000);
// *** Charger controller ***
static BQ25700::Controller Charger(i2c);
// *** BMS chip ***
static MAX17205::Controller BMS(i2c);
// *** IPC Mananger ***
static IPC::Manager ipc_manager(IPC::Check::PID<IPC::BMS>());
// *** IPC Interface ***
static IPC::BMS_Interface ipc_interface;

// Extern LED_RED PIN
static GPIO::PIN Led_Red(GPIO::Port::B, 1, GPIO::Mode::Output);

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
        if (CHR_OK.get_state())
        {
            // Input power is present, set charge current
            if(Charger.set_charge_current(1000))
                Led_Red.set_state(true);
            else
                Led_Red.set_state(false);
        }
        else
        {
            // Turn off red LED
        Led_Red.set_state(false);
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
    SCL.set_alternate_function(IO::I2C_1);
    SDA.set_alternate_function(IO::I2C_1);
    i2c.enable();

    // Initialize the charger interface
    CHR_OK.set_pull(GPIO::Pull::Pull_Up);
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
void IPC::BMS_Interface::sleep(void)
{
    // Disable the pullup to save power
    CHR_OK.set_pull(GPIO::Pull::No_Pull);
};

/**
 * @brief Set everything up after waking up
 */
void IPC::BMS_Interface::wake(void)
{
    // Enable the pullup to sense whether an input is present
    CHR_OK.set_pull(GPIO::Pull::Pull_Up);
};

/**
 * @brief Get the latest voltage measurement of the battery
 * @return The battery voltage in [mV]
 */
unsigned int IPC::BMS_Interface::get_battery_voltage(void) const
{
    return ::BMS.get_battery_voltage();
};

/**
 * @brief Get the latest current measurement of the battery
 * @return The battery current in [mA]
 */
signed int IPC::BMS_Interface::get_battery_current(void) const
{
    return ::BMS.get_battery_current();
};

/**
 * @brief Get the current state of the charger.
 * @return The state of the charger. (Init, Idle, Charging, OTG, Error)
 */
// BQ25700::State BMS_Interface::get_charger_state(void) const
// {
//     return Charger.get_state();
// };