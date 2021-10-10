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
 * @version v1.0.0
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
    // Schedule Threads
    OS.scheduleThread(&Task_System , OTOS::Check::StackSize<512>(), OTOS::PrioHigh);
    OS.scheduleThread(&Task_Display, OTOS::Check::StackSize<512>(), OTOS::PrioNormal);
    OS.scheduleThread(&Task_BMS    , OTOS::Check::StackSize<512>(), OTOS::PrioNormal);
    OS.scheduleThread(&Task_PD     , OTOS::Check::StackSize<512>(), OTOS::PrioNormal);

    // Start the task execution
    OS.start();

    // Never reached
    return 0;
};
