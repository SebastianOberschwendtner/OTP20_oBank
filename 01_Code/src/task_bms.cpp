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
#include "task_bms.h"
#include "drivers.h"
#include "task.h"
#include "configuration.h"

namespace {
    // === Global data within task ===
    // *** I/O pins ***
    GPIO::PIN CHR_OK(GPIO::Port::A, 2, GPIO::Mode::Input);
    GPIO::PIN EN_OTG(GPIO::Port::B, 5, GPIO::Mode::Output);
    GPIO::PIN SCL(GPIO::Port::A, 9);
    GPIO::PIN SDA(GPIO::Port::A, 10);
    // *** i2c controller ***
    I2C::Controller i2c(IO::I2C_1, 100000);
    // *** Charger controller ***
    BQ25700::Controller Charger(i2c);
    // *** BMS chip ***
    MAX17205::Controller Gauge(i2c);
    // *** IPC Mananger ***
    IPC::Manager ipc_manager(IPC::Check::PID<IPC::BMS>());
    // *** IPC Interface ***
    IPC::BMS_Interface ipc_interface;
    IPC::PD_Interface *pd_interface;

    // Extern LED_RED PIN
    GPIO::PIN Led_Red(GPIO::Port::B, 1, GPIO::Mode::Output);

    // === Functions ===

    /**
     * @brief Initialize the BMS task.
     */
    void initialize()
    {
        // Setup the IPC interface
        ipc_manager.register_data(&ipc_interface);
        pd_interface = IPC::wait_for_data<IPC::PD_Interface>(IPC::PD);

        // Set up the i2c interface
        SCL.set_alternate_function(IO::I2C_1);
        SDA.set_alternate_function(IO::I2C_1);
        i2c.enable();

        // Initialize the charger interface
        CHR_OK.set_pull(GPIO::Pull::Pull_Up);
        CHR_OK.enable_interrupt(GPIO::Edge::Rising);
        Charger.initialize();
        Charger.set_OTG_voltage(5000);
        Charger.set_OTG_current(2000);

        // Initialize the BMS Chip
        Gauge.initialize();

        OTOS::Task::yield();
    };
}; // namespace

/**
 * @brief Main task for handling the BMS
 */
void Task_BMS()
{
    // Setup stuff
    initialize();

    // Setup the message router
    BMS::Handler handler(IPC::PID::BMS); // NOLINT

    // Start looping
    while(true)
    {
        // Read battery data
        Gauge.read_battery_current_avg();
        Gauge.read_battery_voltage();
        Gauge.read_cell_voltage();
        Gauge.read_remaining_capacity();
        Gauge.read_soc();
        Gauge.read_TTE();
        Gauge.read_TTF();

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
            // Enable OTG if requested
            if (EN_OTG.get_state())
                Charger.enable_OTG(true);
            else
                Charger.enable_OTG(false);

            // Turn off red LED
            Led_Red.set_state(false);
        }

        // Handle the command queue
        while(not ipc_interface.command_queue.empty())
        {
            handler.receive(ipc_interface.command_queue.front().get());
            ipc_interface.command_queue.pop();
        }

        OTOS::Task::yield();
    };
};

extern "C" void EXTI2_3_IRQHandler(void)
{
    CHR_OK.reset_pending_interrupt();
};

// === IPC interface ===

/**
 * @brief Set everything up for sleepmode
 */
void IPC::BMS_Interface::sleep()
{
    // Disable the pullup to save power
    CHR_OK.set_pull(GPIO::Pull::No_Pull);
};

/**
 * @brief Set everything up after waking up
 */
void IPC::BMS_Interface::wake()
{
    // Enable the pullup to sense whether an input is present
    CHR_OK.set_pull(GPIO::Pull::Pull_Up);
};

/**
 * @brief Get the latest voltage measurement of the battery
 * @return The battery voltage in [mV]
 */
unsigned int IPC::BMS_Interface::get_battery_voltage() const
{
    return ::Gauge.get_battery_voltage();
};

/**
 * @brief Get the latest current measurement of the battery
 * @return The battery current in [mA]
 */
signed int IPC::BMS_Interface::get_battery_current() const
{
    return ::Gauge.get_battery_current();
};

/**
 * @brief Get the latest cell voltage measurement of the battery.
 * @param cell The cell number to get the voltage for (1 or 2).
 * @return The battery current in [mA]
 */
unsigned int IPC::BMS_Interface::get_cell_voltage(unsigned char cell) const
{
    return ::Gauge.get_cell_voltage(cell);
};

/**
 * @brief Get the remaining battery capacity as reported by the BMS chip.
 * 
 * @return The remaining battery capacity in [mAh]
 */
unsigned int IPC::BMS_Interface::get_remaining_capacity() const
{
    return ::Gauge.get_remaining_capacity();
};

/**
 * @brief Get the SOC as reported by the BMS chip.
 * 
 * @return The SOC in 0.1[%].
 */
unsigned int IPC::BMS_Interface::get_soc() const
{
    return ::Gauge.get_SOC();
};

/**
 * @brief Get the estimated time to empty as reported by the BMS chip.
 * 
 * @return  The time to empty in [s].
 */
unsigned int IPC::BMS_Interface::get_time2empty() const
{
    return ::Gauge.get_TTE();
};

/**
 * @brief Get the estimated time to full as reported by the BMS chip.
 * 
 * @return  The time to full in [s].
 */
unsigned int IPC::BMS_Interface::get_time2full() const
{
    return ::Gauge.get_TTF();
};

/**
 * @brief Get the current state of the charger by checking whether
 * the 5V input power is present.
 * @return Returns true if the charger is connected, false otherwise.
 */
bool IPC::BMS_Interface::is_charging() const
{
    return ::CHR_OK.get_state();
};

/**
 * @brief Set the OTG voltage when receiving the Voltage command.
 * @param msg Voltage message which contains the voltage to set.
 */
void BMS::Handler::on_receive(const IPC::Command::Voltage &msg) const
{
    if (msg.voltage > 0)
        Charger.set_OTG_voltage(msg.voltage);
    else
        Charger.set_OTG_voltage(5000);
};

/**
 * @brief Set the OTG current when receiving the Current command.
 * @param msg Voltage message which contains the current to set.
 */
void BMS::Handler::on_receive(const IPC::Command::Current &msg) const
{
    if (msg.current > 0)
        Charger.set_OTG_current(msg.current);
    else
        Charger.set_OTG_current(500);
};