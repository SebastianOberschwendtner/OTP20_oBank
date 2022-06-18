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
 * @file    task_system.cpp
 * @author  SO
 * @version v2.0.0
 * @date    16-September-2021
 * @brief   Main system task of the oBank.
 ******************************************************************************
 */

// === Includes ===
#include "tasks.h"
#include "kernel.h"
#include "system.h"
#include <string>

// === Global data within task ===
// *** I/O pins ***
static GPIO::PIN Led_Green{GPIO::Port::B, 0, GPIO::Mode::Output};
static GPIO::PIN Led_Red{GPIO::Port::B, 1, GPIO::Mode::Output};
static GPIO::PIN EN_5V{GPIO::Port::B, 6, GPIO::Mode::Output};
static GPIO::PIN Button{GPIO::Port::A, 0, GPIO::Mode::Input};

// *** IPC Interface to other tasks ***
static IPC::Display_Interface *task_display;
static IPC::BMS_Interface *task_bms;
static IPC::PD_Interface *task_pd;

// *** System Actions and Events ***
static System::Actions actions;
static System::Events events{
    User::Timeout::Display,
    User::Timeout::Button_Hold,
    Button};

// *** The System State Machine ***
static etl::state_chart_ct<
    System::Actions, actions,
    System::TransitionTable, std::size(System::TransitionTable),
    System::StateTable, std::size(System::StateTable),
    System::State_ID::Initialize>
    state_machine;

// === System Task ===
/**
 * @brief Main task for system stuff of the oBank
 */
void Task_System(void)
{
    // Start the state machine which should call the initialize function
    state_machine.start();

    // Start looping
    while (1)
    {
        // Handle the state machine
        auto event = events.get_event(std::chrono::milliseconds(OTOS::get_time_ms()));
        if (event)
            state_machine.process_event(event.value());

        // Indicate the 5V pin state with the green led
        Led_Green.set_state(EN_5V.get_state());

        // and yield
        OTOS::Task::yield();
    };
};

/**
 * @brief Configure the sleep mode.
 */
static void configure_sleep(void)
{
    // Setup the Sleep mode
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CSR = 0;
    // PWR->CR = 0;
    PWR->CR |= PWR_CR_ULP | PWR_CR_LPSDSR; // V_{REFINT} is off in low-power mode
    // PWR->CR |= PWR_CR_PDDS;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
};

/**
 * @brief Get the IPC data needed for task execution
 */
static void get_ipc(void)
{
    // Display Task
    while (!IPC::Manager::get_data(IPC::Display))
        OTOS::Task::yield();
    task_display = static_cast<IPC::Display_Interface *>(IPC::Manager::get_data(IPC::Display).value());

    // BMS Task
    while (!IPC::Manager::get_data(IPC::BMS))
        OTOS::Task::yield();
    task_bms = static_cast<IPC::BMS_Interface *>(IPC::Manager::get_data(IPC::BMS).value());

    // PD Task
    while (!IPC::Manager::get_data(IPC::PD))
        OTOS::Task::yield();
    task_pd = static_cast<IPC::PD_Interface *>(IPC::Manager::get_data(IPC::PD).value());
};

/**
 * @brief Initialize the system task.
 */
void System::Actions::Initialize(void)
{
    // Enable the interrupt for button input to wake up from sleep
    Button.enable_interrupt(GPIO::Edge::Rising);
    // Make sure that 5V Output is disabled
    EN_5V.set_low();
    // Configure the sleep mode
    configure_sleep();
    // Get the IPC data
    get_ipc();

    // Yield for other tasks
    OTOS::Task::yield();
};

/**
 * @brief Configure every task to sleep and then goto sleep mode.
 */
void System::Actions::Sleep(void)
{
    // Turn of all LEDs
    Led_Green.set_low();

    // put all other tasks to sleep
    task_display->sleep();
    task_bms->sleep();
    task_pd->sleep();

    // This enters the sleep mode of the ARM
    __WFI();
};

/**
 * @brief Wake up the system from sleep mode.
 */
void System::Actions::Wake_Up(void)
{
    // wake up all other tasks
    task_display->wake();
    task_bms->wake();
    task_pd->wake();
};

/**
 * @brief Handle the user input according to the button state.
 *
 * This function is called when the input timeout after a
 * button press is reached. When the button is still high,
 * the 5V output is toggled. When the button is low again,
 * the GUI page is changed.
 */
void System::Actions::Handle_User_Input(void)
{
    // Toggle the 5V output when the button is pressed
    if (Button.get_state())
        EN_5V.toggle();

    // Change the GUI page
    else
        task_display->next_page();
};

/**
 * @brief Reset all timeouts to current system time.
 */
void System::Actions::Reset_Timeouts(void)
{
    events.reset_timeouts(
        std::chrono::milliseconds(OTOS::get_time_ms()));
};

/**
 * @brief Check whether battery is currently charging.
 * @return Returns true if the battery is not charging.
 */
bool System::Actions::not_charging(void)
{
    return !task_bms->is_charging();
};

/**
 * @brief Interrupt handler for wake up interrupt.
 */
extern "C" void EXTI0_1_IRQHandler(void)
{
    Button.reset_pending_interrupt();
};
