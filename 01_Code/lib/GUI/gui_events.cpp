/**
 * OTP20 - oBank
 * Copyright (c) 2022 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    gui_events.cpp
 * @author  SO
 * @version v3.1.0
 * @date    18-June-2021
 * @brief   The gui events of the oBank.
 ******************************************************************************
 */

// === Includes ===
#include "gui_events.h"

// === Functions ===

/**
 * @brief Check whether the next page event is triggered.
 *
 * When the event is triggered, the event is automatically reset when calling
 * this function!
 *
 * @return Returns true when the event is triggered.
 */
auto Event::Next_Page::is_triggered() -> bool
{
    // read the state of the event
    const bool state = this->event_active;

    // A read resets the state
    this->event_active = false;

    // return the state
    return state;
};

/**
 * @brief Manually trigger the next page event.
 */
void Event::Next_Page::trigger()
{
    // Set the state of the event
    this->event_active = true;
};