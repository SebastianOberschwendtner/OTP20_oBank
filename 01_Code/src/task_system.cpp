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
 * @version v1.0.0
 * @date    16-September-2021
 * @brief   Main system task of the oBank.
 ******************************************************************************
 */

// === Includes ===
#include "task_system.h"

// === Global data within task ===
// *** I/O pins ***
static GPIO::PIN Led_Green(GPIO::PORTB, GPIO::PIN0, GPIO::OUTPUT);
static GPIO::PIN Led_Red(GPIO::PORTB, GPIO::PIN1, GPIO::OUTPUT);
static GPIO::PIN EN_5V(GPIO::PORTB, GPIO::PIN6, GPIO::OUTPUT);
static GPIO::PIN Button(GPIO::PORTA, GPIO::PIN0, GPIO::INPUT);
// *** IPC Interface to other tasks ***
Display_Interface* task_display;
BMS_Interface* task_bms;
PD_Interface* task_pd;

// === Functions ===
static void initialize(void);
static void configure_sleep(void) ;
static void get_ipc(void);

/**
 * @brief Main task for system stuff of the oBank
 */
void Task_System(void)
{
    // Setup stuff
    initialize();

    // *** Sleep Mode ***
    configure_sleep();

    // *** Get IPC data ***
    get_ipc();

    unsigned long counter = 0;
    // Start looping
    while(1)
    {
        Led_Green.set(EN_5V.get());
        Button.read_edge();
        if (Button.rising_edge())
        {
            counter = 0;
            EN_5V.toggle();
        }
        counter++;
        if(counter > 300)
        {
            counter = 0;
            Led_Green.setLow();
            task_display->sleep();
            task_bms->sleep();
            task_pd->sleep();
            __WFI();
            task_display->wake();
            task_bms->wake();
            task_pd->wake();
            Button.read_edge();
        }

        OTOS::Task::yield();
    };
};

/**
 * @brief Initialize the system task.
 */
static void initialize(void)
{
    // Enable the interrupt for button input to wake up from sleep
    Button.enable_interrupt(GPIO::Edge::Rising);
    // Make sure that 5V Output is disabled
    EN_5V.setLow();

    // Yield for other tasks
    OTOS::Task::yield();
};

/**
 * @brief Configure the sleepmode.
 */
static void configure_sleep(void)
{
    // Setup the Sleep mode
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CSR = 0;
    // PWR->CR = 0;
    PWR->CR |= PWR_CR_ULP | PWR_CR_LPSDSR;   // V_{REFINT} is off in low-power mode
    // PWR->CR |= PWR_CR_PDDS;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
};

/**
 * @brief Get the IPC data needed for task execution
 */
static void get_ipc(void)
{
    // Display Task
    while (!IPC::Manager::get_data(PID::Display)) OTOS::Task::yield(); 
    task_display = static_cast<Display_Interface*>(IPC::Manager::get_data(PID::Display).value());

    // BMS Task
    while (!IPC::Manager::get_data(PID::BMS)) OTOS::Task::yield(); 
    task_bms = static_cast<BMS_Interface*>(IPC::Manager::get_data(PID::BMS).value());

    // PD Task
    while (!IPC::Manager::get_data(PID::PD)) OTOS::Task::yield(); 
    task_pd = static_cast<PD_Interface*>(IPC::Manager::get_data(PID::PD).value());
};

/**
 * @brief Interrupt handler for wake up interrupt.
 */
extern "C" void EXTI0_1_IRQHandler(void)
{
    Button.reset_pending_interrupt();
};
