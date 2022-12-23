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
static GPIO::PIN EN_OTG(GPIO::Port::B, 5, GPIO::Mode::Output);
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
        BMS.read_cell_voltage();
        BMS.read_remaining_capacity();
        BMS.read_soc();
        BMS.read_TTE();
        BMS.read_TTF();

        // Manage charging
        if (CHR_OK.get_state())
        {
            EN_OTG.set_low();
            Charger.enable_OTG(false);
            // Input power is present, set charge current
            if(Charger.set_charge_current(User::Power::Max_Charge_Current))
                Led_Red.set_state(true);
            else
                Led_Red.set_state(false);
        }
        else
        {
            // EN_OTG.set_high();
            // Charger.enable_OTG(true);
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
    EN_OTG.set_low();
    Charger.initialize();
    Charger.set_OTG_voltage(5000);
    Charger.set_OTG_current(500);

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
 * @brief Get the latest cell voltage measurement of the battery.
 * @param cell The cell number to get the voltage for (1 or 2).
 * @return The battery current in [mA]
 */
unsigned int IPC::BMS_Interface::get_cell_voltage(unsigned char cell) const
{
    return ::BMS.get_cell_voltage(cell);
};

/**
 * @brief Get the remaining battery capacity as reported by the BMS chip.
 * 
 * @return The remaining battery capacity in [mAh]
 */
unsigned int IPC::BMS_Interface::get_remaining_capacity(void) const
{
    return ::BMS.get_remaining_capacity();
};

/**
 * @brief Get the SOC as reported by the BMS chip.
 * 
 * @return The SOC in 0.1[%].
 */
unsigned int IPC::BMS_Interface::get_soc(void) const
{
    return ::BMS.get_SOC();
};

/**
 * @brief Get the estimated time to empty as reported by the BMS chip.
 * 
 * @return  The time to empty in [s].
 */
unsigned int IPC::BMS_Interface::get_time2empty(void) const
{
    return ::BMS.get_TTE();
};

/**
 * @brief Get the estimated time to full as reported by the BMS chip.
 * 
 * @return  The time to full in [s].
 */
unsigned int IPC::BMS_Interface::get_time2full(void) const
{
    return ::BMS.get_TTF();
};

/**
 * @brief Get the current state of the charger by checking whether
 * the 5V input power is present.
 * @return Returns true if the charger is connected, false otherwise.
 */
bool IPC::BMS_Interface::is_charging(void) const
{
    return ::CHR_OK.get_state();
};