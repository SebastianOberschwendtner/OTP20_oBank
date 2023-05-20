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
 * @version v3.1.0
 * @date    14-September-2021
 * @brief   Display task of the oBank
 ******************************************************************************
 */

// === Includes ===
#include "drivers.h"
#include "task.h"
#include "interprocess.h"
#include "configuration.h"
#include "gui.h"

// The canvas object
extern Graphics::Canvas_BW canvas;

namespace
{
    // === Global data within task ===
    // *** I/O pins ***
    GPIO::PIN SCL(GPIO::Port::A, 8);
    GPIO::PIN SDA(GPIO::Port::B, 4);
    // *** i2c controller ***
    I2C::Controller i2c(IO::I2C_3, 100000);
    // *** Display controller ***
    SSD1306::Controller Display(i2c);
    // *** IPC Mananger ***
    IPC::Manager ipc_manager(IPC::Check::PID<IPC::Display>());
    // *** IPC Interface ***
    IPC::Display_Interface ipc_interface;
    // *** IPC task references
    // IPC::BMS_Interface *task_bms;
    // IPC::PD_Interface *task_pd;

    // *** GUI Actions and Events ***
    GUI::Actions actions;
    GUI::Events events;

    // *** The GUI State Machine ***
    etl::state_chart_ct<
        GUI::Actions, actions,
        GUI::TransitionTable, std::size(GUI::TransitionTable),
        GUI::StateTable, std::size(GUI::StateTable),
        GUI::State_ID::Main_Info>
        state_machine;

    // === Functions ===

    /**
     * @brief Initialize the task
     */
    void initialize()
    {
        // Register the IPC interface
        ipc_manager.register_data(&ipc_interface);
        // task_bms = IPC::wait_for_data<IPC::BMS_Interface>(IPC::BMS);
        // task_pd = IPC::wait_for_data<IPC::PD_Interface>(IPC::PD);

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
}; // namespace

/**
 * @brief Main task for handling the display interface
 */
void Task_Display()
{
    // Setup stuff
    initialize();

    // Start the state machine
    state_machine.start();

    // Start looping
    while (true)
    {
        // Handle the state machine
        auto event = events.get_event();
        state_machine.process_event(event);

        // Send the display buffer to the display
        Display.draw(GUI::get_data_pointer());
        OTOS::Task::yield();
    };
};

// === IPC interface ===

/**
 * @brief Set everything up for sleep mode
 */
void IPC::Display_Interface::sleep()
{
    ::Display.off();
};

/**
 * @brief Set everything up after waking up
 */
void IPC::Display_Interface::wake()
{
    ::Display.on();
};

/**
 * @brief Increase the display page
 */
void IPC::Display_Interface::next_page()
{
    // Trigger the next page event
    events.next_page.trigger();
};

// === GUI Actions ===

/**
 * @brief Draw the main info page
 */
void GUI::Actions::Draw_Main_Info() // NOLINT(*-convert-member-functions-to-static)
{
    // Draw the main info
    GUI::draw_main_info(
        IPC::BMS_Interface::get_battery_voltage(),
        IPC::BMS_Interface::get_battery_current()
    );
};

/**
 * @brief Draw the status info page
 */
void GUI::Actions::Draw_Status_Info() // NOLINT(*-convert-member-functions-to-static)
{
    GUI::draw_state_info(
        IPC::BMS_Interface::outputs_enabled(),
        IPC::BMS_Interface::is_charging());
};

/**
 * @brief Draw the cell info page
 */
void GUI::Actions::Draw_Cell_Info() // NOLINT(*-convert-member-functions-to-static)
{
    GUI::draw_cell_info(
        IPC::BMS_Interface::get_cell_voltage(1),
        IPC::BMS_Interface::get_cell_voltage(2)
    );
};

/**
 * @brief Draw the SOC info.
 */
void GUI::Actions::Draw_SOC_Info() // NOLINT(*-convert-member-functions-to-static)
{
    GUI::draw_soc_info(
        IPC::BMS_Interface::get_remaining_capacity(),
        IPC::BMS_Interface::get_soc()
    );
};

/**
 * @brief Draw the charge/discharge times.
 */
void GUI::Actions::Draw_Time_Info() // NOLINT(*-convert-member-functions-to-static)
{
    GUI::draw_time_info(
        IPC::BMS_Interface::get_time2empty(),
        IPC::BMS_Interface::get_time2full()
    );
};

/**
 * @brief Draw the USB PD info.
 */
void GUI::Actions::Draw_PD_Info() // NOLINT(*-convert-member-functions-to-static)
{
    GUI::clear_canvas();
    canvas.set_font(Font::_8px::Default);
    canvas.set_cursor(0, 0);
    canvas << "PDO U: " << IPC::PD_Interface::get_voltage() << " mV" << OTOS::endl;
    canvas << "PDO I: " << IPC::PD_Interface::get_current() << " mA" << OTOS::endl;
};

/**
 * @brief Clear the display canvas
 */
void GUI::Actions::Clear_Buffer() // NOLINT(*-convert-member-functions-to-static)
{
    canvas.set_font(Font::_16px::Default);
    // GUI::clear_canvas();
};