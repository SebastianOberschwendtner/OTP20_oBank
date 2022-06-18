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
static GPIO::PIN EN_5V{GPIO::Port::B, 6, GPIO::Mode::Output};
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

// *** GUI Actions and Events ***
static GUI::Actions actions;
static GUI::Events events;

// *** The GUI State Machine ***
static etl::state_chart_ct<
    GUI::Actions, actions,
    GUI::TransitionTable, std::size(GUI::TransitionTable),
    GUI::StateTable, std::size(GUI::StateTable),
    GUI::State_ID::Main_Info>
    state_machine;

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

    // Start the state machine
    state_machine.start();

    // Start looping
    while (1)
    {
        // Handle the state machine
        auto event = events.get_event();
        state_machine.process_event(event);

        // Send the display buffer to the display
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
    // Trigger the next page event
    events.next_page.trigger();
};

// === GUI Actions ===

/**
 * @brief Draw the main info page
 */
void GUI::Actions::Draw_Main_Info(void)
{
    // Draw the main info
    GUI::draw_main_info(
        task_bms->get_battery_voltage(),
        task_bms->get_battery_current()
    );
};

/**
 * @brief Draw the status info page
 */
void GUI::Actions::Draw_Status_Info(void)
{
    GUI::draw_state_info(
        EN_5V.get_state(), 
        task_bms->is_charging());
};

/**
 * @brief Draw the cell info page
 */
void GUI::Actions::Draw_Cell_Info(void)
{
    GUI::draw_cell_info(
        task_bms->get_cell_voltage(1),
        task_bms->get_cell_voltage(2)
    );
};

/**
 * @brief Clear the display canvas
 */
void GUI::Actions::Clear_Buffer(void)
{
    // GUI::clear_canvas();
};