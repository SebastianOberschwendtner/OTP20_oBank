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
 * @version v3.0.0
 * @date    09-October-2021
 * @brief   Task for handling the Power Delivery of the oBank
 ******************************************************************************
 */

// === Includes ===
#include "drivers.h"
#include "task.h"
#include "interprocess.h"
#include "configuration.h"
#include <algorithm>

// === Global data within task ===
namespace {
    // *** I/O pins ***
    GPIO::PIN SCL(GPIO::Port::A, 9);
    GPIO::PIN SDA(GPIO::Port::A, 10);
    GPIO::PIN EN_OTG(GPIO::Port::B, 5, GPIO::Mode::Output);
    GPIO::PIN EN_5V_AUX(GPIO::Port::B, 7, GPIO::Mode::Output);
    // *** i2c controller ***
    I2C::Controller i2c(IO::I2C_1, 100000);
    // *** PD controller ***
    TPS65987::Controller PD(i2c);
    TPS65987::PDO pdo;
    // *** IPC Mananger ***
    IPC::Manager ipc_manager(IPC::Check::PID<IPC::PD>());
    // *** IPC Interface ***
    IPC::PD_Interface ipc_interface;
    IPC::BMS_Interface *bms_interface;

    // === Functions ===
    /**
     * @brief Initialize the PD task.
     */
    void initialize()
    {
        // Setup the IPC interface
        ipc_manager.register_data(&ipc_interface);
        bms_interface = IPC::wait_for_data<IPC::BMS_Interface>(IPC::BMS);

        // Set up the i2c interface
        SCL.set_alternate_function(IO::I2C_1);
        SDA.set_alternate_function(IO::I2C_1);
        i2c.enable();

        // Initialize the charger interface
        EN_5V_AUX.set_low();
        PD.initialize();

        OTOS::Task::yield();
    };
}; // namespace

/**
 * @brief Main task for handling the display interface
 */
void Task_PD()
{
    using namespace TPS65987;

    // Setup stuff
    initialize();

    // Registers to write
    const class Status status;
    class GlobalConfiguration global_config;
    class PortConfiguration port_config;
    const class PowerPathStatus power_status;
    class PortControl port_control;
    class PDO vSafe5V;
    class PDO previous;

    // Set the 5V capability
    vSafe5V.set_voltage(5000);
    vSafe5V.set_current(500);
    PD.buffer_data.fill(0);
    PD.register_TX_source_capability({vSafe5V, 1});
    vSafe5V.set_voltage(5000);
    vSafe5V.set_current(2000);
    PD.register_TX_source_capability({vSafe5V, 1});
    vSafe5V.set_voltage(9000);
    vSafe5V.set_current(2000);
    PD.register_TX_source_capability({vSafe5V, 1});
    PD.buffer_data[3] = 0b1100;
    PD.write_register(Register::TX_Source_Cap);

    // Set the Global Configuration
    PD.read(global_config);
    global_config.set_PP2config(1);
    PD.write(global_config);

    // Set the Port Configuration
    PD.read(port_config);
    port_config.set_TypeCStateMachine(0b01);
    PD.write(port_config);

    // Set the port control
    PD.read(port_control);
    port_control.set_ChargerAdvertiseEnable(0b000);
    PD.write(port_control);

    // Start looping
    while(true)
    {
        // Select the the state machine when OTG state changes
        EN_OTG.read_edge();
        if (EN_OTG.rising_edge())
        {
            PD.read(port_config);
            port_config.set_TypeCStateMachine(0b01);
            PD.write(port_config);
        }
        else if (EN_OTG.falling_edge())
        {
            PD.read(port_config);
            port_config.set_TypeCStateMachine(0b00);
            PD.write(port_config);
        }

        // read the current PDO
        pdo = PD.read_active_pdo().value();
        if (pdo.voltage() != previous.voltage())
        {
            bms_interface->push_command(IPC::Command::Voltage{ pdo.voltage() });
            bms_interface->push_command(IPC::Command::Current{ pdo.current() });
        }
        previous = pdo;
        OTOS::Task::yield();
    };
};

// === IPC interface ===

/**
 * @brief Set everything up for sleepmode
 */
void IPC::PD_Interface::sleep()
{
};

/**
 * @brief Set everything up after waking up
 */
void IPC::PD_Interface::wake()
{
};

/**
 * @brief Return the current PDO voltage.
 * @return The current PDO voltage in [mV]
 */
auto IPC::PD_Interface::get_voltage() -> uint16_t
{
    return pdo.voltage();
};

/**
 * @brief Return the current PDO current.
 * @return The current PDO current in [mA]
 */
auto IPC::PD_Interface::get_current() -> uint16_t
{
    return pdo.current();
};