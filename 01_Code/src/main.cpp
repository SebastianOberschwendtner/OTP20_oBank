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
 * @file    main.cpp
 * @author  SO
 * @version v2.1.0
 * @date    14-September-2021
 * @brief   Main firmware for the oBank.
 ******************************************************************************
 */

// === Includes ===
#include "main.h"

// === Functions ===
OTOS::Kernel OS;

// === Main ===
int main(void)
{
    // Configure the SysTick timer to generate an interrupt every 1ms
    Timer::SysTick_Configure();

    // Schedule Threads
    OS.schedule_thread<512>(&Task_System , OTOS::Priority::High, 5);
    OS.schedule_thread<512>(&Task_Display, OTOS::Priority::Normal);
    OS.schedule_thread<512>(&Task_BMS    , OTOS::Priority::Normal);
    OS.schedule_thread<512>(&Task_PD     , OTOS::Priority::Normal);

    // Start the task execution
    OS.start();

    // Never reached
    return 0;
};

/** 
 * @brief Provide a Interrupt handler for the systick timer,
 * which gets called every 1 ms.
 */
extern "C" void SysTick_Handler(void)
{
    OS.update_schedule();
    OS.count_time_ms();
};
