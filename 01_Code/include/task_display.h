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

#ifndef TASK_DISPLAY_H_
#define TASK_DISPLAY_H_

// === Includes ===
#include <stdio.h>
#include "task.h"
#include "ipc.h"
#include "pid.h"
#include "drivers.h"
#include "graphics.h"

// === Other Tasks ===
#include "task_bms.h"

// === Task object ===
class Display_Interface
{
    public:
    // *** Constructor ***
    Display_Interface() {};

    // *** Methods ***
    void    sleep(void);
    void    wake(void);
};

// === Declarations ===
void        Task_Display    (void);

#endif
