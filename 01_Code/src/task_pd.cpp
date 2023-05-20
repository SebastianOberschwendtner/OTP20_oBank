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
 * @version v3.1.0
 * @date    09-October-2021
 * @brief   Task for handling the Power Delivery of the oBank
 ******************************************************************************
 */

// === Includes ===
#include "task_pd.h"
#include "configuration.h"
#include "drivers.h"
#include "interprocess.h"
#include "task.h"
#include <algorithm>

// === Global data within task ===
namespace
{
    // *** I/O pins ***
    GPIO::PIN SCL(GPIO::Port::A, 9);
    GPIO::PIN SDA(GPIO::Port::A, 10);
    GPIO::PIN EN_5V_AUX(GPIO::Port::B, 7, GPIO::Mode::Output);
    // *** i2c controller ***
    I2C::Controller i2c(IO::I2C_1, 100000);
    // *** PD controller ***
    TPS65987::Controller PD_Controller(i2c);
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
        // Disable the 5V AUX
        EN_5V_AUX.set_low();

        // Setup the IPC interface
        ipc_manager.register_data(&ipc_interface);
        bms_interface = IPC::wait_for_data<IPC::BMS_Interface>(IPC::BMS);

        // Set up the i2c interface
        SCL.set_alternate_function(IO::I2C_1);
        SDA.set_alternate_function(IO::I2C_1);
        i2c.enable();

        // Initialize the charger interface
        PD_Controller.initialize();

        // Registers to write
        TPS65987::GlobalConfiguration global_config;
        TPS65987::PortConfiguration port_config;
        TPS65987::PortControl port_control;
        TPS65987::PDO pdo_;

        // Set the PDO capabilities
        pdo_.set_voltage(5000);
        pdo_.set_current(500);
        PD_Controller.buffer_data.fill(0);
        PD_Controller.register_TX_source_capability({pdo_, 1});
        pdo_.set_voltage(5000);
        pdo_.set_current(2000);
        PD_Controller.register_TX_source_capability({pdo_, 1});
        pdo_.set_voltage(9000);
        pdo_.set_current(2000);
        PD_Controller.register_TX_source_capability({pdo_, 1});
        PD_Controller.buffer_data[3] = 0b1100;
        PD_Controller.write_register(TPS65987::Register::TX_Source_Cap);

        // Set the Global Configuration
        PD_Controller.read(global_config);
        global_config.set_PP2config(1);
        PD_Controller.write(global_config);

        // Set the Port Configuration
        PD_Controller.read(port_config);
        port_config.set_TypeCStateMachine(0b00); // SINK State machine
        PD_Controller.write(port_config);

        // Set the port control
        PD_Controller.read(port_control);
        port_control.set_ChargerAdvertiseEnable(0b000);
        PD_Controller.write(port_control);

        OTOS::Task::yield();
    };
}; // namespace

/**
 * @brief Main task for handling the display interface
 */
void Task_PD()
{
    // Setup stuff
    initialize();

    // Remember the previous PDO
    TPS65987::PDO previous;

    // Setup the message router
    PD::Handler handler(IPC::PID::PD); // NOLINT

    // Start looping
    while (true)
    {
        // Handle the command queue
        while (not ipc_interface.command_queue.empty())
        {
            handler.receive(ipc_interface.command_queue.front().get());
            ipc_interface.command_queue.pop();
        }

        // Check whether the PDO changed and send it to the BMS
        pdo = PD_Controller.read_active_pdo().value();
        if (pdo.voltage() != previous.voltage())
        {
            bms_interface->push_command(IPC::Command::Voltage{pdo.voltage()});
            bms_interface->push_command(IPC::Command::Current{pdo.current()});
        }
        previous = pdo;
        OTOS::Task::yield();
    };
};

// === IPC interface ===

/**
 * @brief Set everything up for sleepmode
 */
void IPC::PD_Interface::sleep(){};

/**
 * @brief Set everything up after waking up
 */
void IPC::PD_Interface::wake(){};

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

/**
 * @brief Set the PD state machine according to the command.
 * When enabling the output the Source state machine is used.
 * When disabling the output the Sink state machine is used.
 *
 * @param msg Outputs message which contains the state to set.
 */
void PD::Handler::on_receive(const IPC::Command::Output &msg)
{
    // Shortcut for the output states
    using states = IPC::Command::Output::State;

    // Read the current PD port config
    TPS65987::PortConfiguration port_config;
    PD_Controller.read(port_config);
    auto state_machine = port_config.TypeCStateMachine();

    // Switch according to the state
    switch (msg.state)
    {
        case states::Off:
            state_machine = 0b00;
            break;
        case states::On:
            state_machine = 0b01;
            break;
        case states::Toggle:
            state_machine = (state_machine == 0b00) ? 0b01 : 0b00;
            break;
        default:
            break;
    }

    // Update the port config
    port_config.set_TypeCStateMachine(state_machine);
    PD_Controller.write(port_config);
};