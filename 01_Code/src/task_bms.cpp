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
 * @version v3.1.0
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
    GPIO::PIN ALRT1(GPIO::Port::A, 3, GPIO::Mode::Input);
    GPIO::PIN EN_OTG(GPIO::Port::B, 5, GPIO::Mode::Output);
    GPIO::PIN EN_5V{GPIO::Port::B, 6, GPIO::Mode::Output};
    GPIO::PIN SCL(GPIO::Port::A, 9);
    GPIO::PIN SDA(GPIO::Port::A, 10);
    // *** i2c controller ***
    I2C::Controller i2c(IO::I2C_1, 100000);
    // *** Charger controller ***
    BQ25700::Controller Charger(i2c);
    // *** BMS chip ***
    MAX17205::Controller Battery(i2c);
    // *** IPC Mananger ***
    IPC::Manager ipc_manager(IPC::Check::PID<IPC::BMS>());
    // *** IPC Interface ***
    IPC::BMS_Interface ipc_interface;
    IPC::PD_Interface *pd_interface;

    // === Functions ===
    /**
     * @brief Initialize the BMS task.
     */
    void initialize()
    {
        // Disable all outputs
        EN_OTG.set_state(false);
        EN_5V.set_state(false);

        // Setup the IPC interface
        ipc_manager.register_data(&ipc_interface);
        pd_interface = IPC::wait_for_data<IPC::PD_Interface>(IPC::PD);

        // Set up the i2c interface
        SCL.set_alternate_function(IO::I2C_1);
        SDA.set_alternate_function(IO::I2C_1);
        i2c.enable();

        // Initialize the charger interface
        CHR_OK.set_pull(GPIO::Pull::Pull_Up);
        CHR_OK.enable_interrupt(GPIO::Edge::Both);
        Charger.initialize();
        Charger.set_OTG_voltage(5000);
        Charger.set_OTG_current(2000);
        BQ25700::ChargeOption0 option0;
        Charger.read(option0);
        if (! option0.EN_OOA() )
        {
            option0.set_EN_OOA(true); // Enable Out-of-Audio mode
            Charger.write(option0);
        }

        // Initialize the BMS Chip
        ALRT1.set_pull(GPIO::Pull::Pull_Up);
        ALRT1.enable_interrupt(GPIO::Edge::Both);
        Battery.initialize();
        MAX17205::PackCfg config;
        MAX17205::Config alert;
        MAX17205::SAlrtTh thresholds;
        Battery.read(config);
        Battery.read(alert);
        Battery.read(thresholds);
        if (! config.ChEn() ) // When this flag is not set, it is assumed the chip is not initialized
        {
            // Set the cell configuration
            config.value = (MAX17205::TdEn | MAX17205::ChEn | MAX17205::BALCFG_1 | MAX17205::BALCFG_0 | 2U);
            Battery.write(config);

            // Set the alert thresholds
            thresholds.set_SMIN(User::Power::SoC_minimum);
            thresholds.set_SMAX(0xFF); // Disable max SOC alert
            Battery.write(thresholds);

            // Enable the alerts
            alert.set_ALRTp(false); // ALRT1 pin is active low
            alert.set_Aen(true); // Enable the alerts
            Battery.write(alert);
        }

        OTOS::Task::yield();
    }
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
        // Handle the command queue
        while(not ipc_interface.command_queue.empty())
        {
            handler.receive(ipc_interface.command_queue.front().get());
            ipc_interface.command_queue.pop();
        }

        // Read battery data
        Battery.read_battery_current_avg();
        Battery.read_battery_voltage();
        Battery.read_cell_voltage();
        Battery.read_remaining_capacity();
        Battery.read_soc();
        Battery.read_TTE();
        Battery.read_TTF();

        // Manage charging
        if (CHR_OK.get_state())
        {
            // EN_OTG.set_low();
            // Charger.enable_OTG(false);
            // Input power is present, set charge current
            Charger.set_charge_current(User::Power::Max_Charge_Current);
        }
        else if ( not ALRT1.get_state())
        {
            // battery reports some alert -> disable all outputs
            EN_5V.set_low();
            EN_OTG.set_low();
            Charger.enable_OTG(false);
        }

        OTOS::Task::yield();
    };
}

extern "C" void EXTI2_3_IRQHandler(void)
{
    CHR_OK.reset_pending_interrupt();
    ALRT1.reset_pending_interrupt();
}

// === IPC interface ===

/**
 * @brief Set everything up for sleepmode
 */
void IPC::BMS_Interface::sleep()
{
    // Disable the pullup to save power
    // CHR_OK.set_pull(GPIO::Pull::No_Pull);
}

/**
 * @brief Set everything up after waking up
 */
void IPC::BMS_Interface::wake()
{
    // Enable the pullup to sense whether an input is present
    // CHR_OK.set_pull(GPIO::Pull::Pull_Up);
}

/**
 * @brief Get the latest voltage measurement of the battery
 * @return The battery voltage in [mV]
 */
auto IPC::BMS_Interface::get_battery_voltage() -> uint16_t
{
    return ::Battery.get_battery_voltage().get();
}

/**
 * @brief Get the latest current measurement of the battery
 * @return The battery current in [mA]
 */
auto IPC::BMS_Interface::get_battery_current() -> int16_t
{
    return ::Battery.get_battery_current().get();
}

/**
 * @brief Get the latest cell voltage measurement of the battery.
 * @param cell The cell number to get the voltage for (1 or 2).
 * @return The battery current in [mA]
 */
auto IPC::BMS_Interface::get_cell_voltage(unsigned char cell) -> uint16_t
{
    return ::Battery.get_cell_voltage(cell).get();
}

/**
 * @brief Get the remaining battery capacity as reported by the BMS chip.
 * 
 * @return The remaining battery capacity in [mAh]
 */
auto IPC::BMS_Interface::get_remaining_capacity() -> uint16_t
{
    return ::Battery.get_remaining_capacity().get();
}

/**
 * @brief Get the SOC as reported by the BMS chip.
 * 
 * @return The SOC in 0.1[%].
 */
auto IPC::BMS_Interface::get_soc() -> uint16_t
{
    return ::Battery.get_SOC().get();
}

/**
 * @brief Get the estimated time to empty as reported by the BMS chip.
 * 
 * @return  The time to empty in [s].
 */
auto IPC::BMS_Interface::get_time2empty() -> uint32_t
{
    return ::Battery.get_TTE().get();
}

/**
 * @brief Get the estimated time to full as reported by the BMS chip.
 * 
 * @return  The time to full in [s].
 */
auto IPC::BMS_Interface::get_time2full() -> uint32_t
{
    return ::Battery.get_TTF().get();
}

/**
 * @brief Get the current state of the charger by checking whether
 * the 5V input power is present.
 * @return Returns true if the charger is connected, false otherwise.
 */
auto IPC::BMS_Interface::is_charging() -> bool
{
    return ::CHR_OK.get_state();
}

/**
 * @brief Get the current state of the USB outputs.
 * @return Returns true if the outputs are enabled, false otherwise.
 */
auto IPC::BMS_Interface::outputs_enabled() -> bool
{
    return ::EN_5V.get_state() or ::EN_OTG.get_state();
}

/**
 * @brief Set the OTG voltage when receiving the Voltage command.
 * @param msg Voltage message which contains the voltage to set.
 */
void BMS::Handler::on_receive(const IPC::Command::Voltage &msg) 
{
    if (msg.voltage > 0)
        Charger.set_OTG_voltage(msg.voltage);
    else
        Charger.set_OTG_voltage(5000);
}

/**
 * @brief Set the OTG current when receiving the Current command.
 * @param msg Voltage message which contains the current to set.
 */
void BMS::Handler::on_receive(const IPC::Command::Current &msg) 
{
    if (msg.current > 0)
        Charger.set_OTG_current(msg.current);
    else
        Charger.set_OTG_current(500);
}

/**
 * @brief Handle the Outputs command and set the outputs accordingly.
 * @param msg Outputs message which contains the state to set.
 */
void BMS::Handler::on_receive(const IPC::Command::Output &msg)
{
    // Shortcut for the state enum
    using states = IPC::Command::Output::State;

    // Set the outputs according to the state
    switch (msg.state)
    {
    case states::On:
    case states::Off:
    case states::Toggle:
        // Toggle the 5V output
        EN_5V.toggle();
        // Toggle the OTG output
        EN_OTG.toggle();
        // Enable OTG if requested
        if (EN_OTG.get_state())
            Charger.enable_OTG(true);
        else
            Charger.enable_OTG(false);
        // Toggle the USB-C output
        pd_interface->push_command( IPC::Command::Output(states::Toggle));
    default:
        break;
    }
}