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
 * @version v3.1.0
 * @date    16-September-2021
 * @brief   Main system task of the oBank.
 ******************************************************************************
 */

// === Includes ===
#include <string>
#include "drivers.h"
#include "task.h"
#include "interprocess.h"
#include "configuration.h"
#include "kernel.h"
#include "system.h"

namespace
{
    // === Global data within task ===
    // *** I/O pins ***
    GPIO::PIN Led_Green{GPIO::Port::B, 0, GPIO::Mode::Output};
    GPIO::PIN Led_Red{GPIO::Port::B, 1, GPIO::Mode::Output};
    GPIO::PIN Button{GPIO::Port::A, 0, GPIO::Mode::Input};

    // *** IPC Interface to other tasks ***
    // IPC::Display_Interface *task_display;
    IPC::BMS_Interface *task_bms;
    // IPC::PD_Interface *task_pd;

    // *** System Actions and Events ***
    System::Actions actions;
    System::Events events{
        User::Timeout::Display,
        User::Timeout::Button_Hold,
        Button};

    // *** The System State Machine ***
    etl::state_chart_ct<
        System::Actions, actions,
        System::TransitionTable, std::size(System::TransitionTable),
        System::StateTable, std::size(System::StateTable),
        System::State_ID::Initialize>
        state_machine;

    /**
     * @brief Configure the sleep mode:
     * - Enter Stop Mode
     * - Wake up from any EXTI line
     */
    void configure_sleep()
    {
        // Setup the Sleep mode
        RCC->APB1ENR |= RCC_APB1ENR_PWREN;
        PWR->CSR = 0U;
        // PWR->CR = 0;
        PWR->CR |= PWR_CR_ULP | PWR_CR_LPSDSR; // V_{REFINT} is off in low-power mode
        // PWR->CR |= PWR_CR_PDDS;
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    };
}; // namespace

// === System Task ===
/**
 * @brief Main task for system stuff of the oBank
 */
void Task_System()
{
    // Start the state machine which should call the initialize function
    state_machine.start();

    // Start looping
    while (true)
    {
        // Handle the state machine
        auto event = events.get_event(std::chrono::milliseconds(OTOS::get_time_ms()));
        state_machine.process_event(event);

        // Indicate the general output state with the leds
        Led_Green.set_state(IPC::BMS_Interface::outputs_enabled());
        Led_Red.set_state(IPC::BMS_Interface::is_charging());

        // and yield
        OTOS::Task::yield();
    };
};

/**
 * @brief Initialize the system task.
 */
void System::Actions::Initialize() // NOLINT(*-convert-member-functions-to-static)
{
    // Enable the interrupt for button input to wake up from sleep
    Button.enable_interrupt(GPIO::Edge::Rising);

    // Configure the sleep mode
    configure_sleep();

    // Get the IPC data
    // task_display = IPC::wait_for_data<IPC::Display_Interface>(IPC::Display);
    task_bms = IPC::wait_for_data<IPC::BMS_Interface>(IPC::BMS);
    // task_pd = IPC::wait_for_data<IPC::PD_Interface>(IPC::PD);

    // Yield for other tasks
    OTOS::Task::yield();
};

/**
 * @brief Configure every task to sleep and then goto sleep mode.
 */
void System::Actions::Sleep() // NOLINT(*-convert-member-functions-to-static)
{
    // Turn of green Led -> Keep red led on when charging
    Led_Green.set_low();

    // Put all other tasks to sleep
    IPC::Display_Interface::sleep();
    IPC::BMS_Interface::sleep();
    IPC::PD_Interface::sleep();

    // This enters the sleep mode of the ARM
    __WFI();
};

/**
 * @brief Wake up the system from sleep mode.
 */
void System::Actions::Wake_Up() // NOLINT(*-convert-member-functions-to-static)
{
    // Wake up all other tasks
    IPC::Display_Interface::wake();
    IPC::BMS_Interface::wake();
    IPC::PD_Interface::wake();
};

/**
 * @brief Handle the user input according to the button state.
 *
 * This function is called when the input timeout after a
 * button press is reached. When the button is still high,
 * the 5V output is toggled. When the button is low again,
 * the GUI page is changed.
 */
void System::Actions::Handle_User_Input() // NOLINT(*-convert-member-functions-to-static)
{
    // Toggle both output states when the button is pressed
    if (Button.get_state())
    {
        task_bms->push_command(
            IPC::Command::Output{IPC::Command::Output::State::Toggle}
        );
    }
    else // Change the GUI page
        IPC::Display_Interface::next_page();
};

/**
 * @brief Reset all timeouts to current system time.
 */
void System::Actions::Reset_Timeouts() // NOLINT(*-convert-member-functions-to-static)
{
    events.reset_timeouts(
        std::chrono::milliseconds(OTOS::get_time_ms()));
};

/**
 * @brief Check whether battery is currently charging.
 * @return Returns true if the battery is not charging.
 */
auto System::Actions::not_charging() -> bool // NOLINT(*-convert-member-functions-to-static)
{
    return !IPC::BMS_Interface::is_charging();
};

/**
 * @brief Interrupt handler for wake up interrupt.
 */
extern "C" void EXTI0_1_IRQHandler(void)
{
    Button.reset_pending_interrupt();
};
