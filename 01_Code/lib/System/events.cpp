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
 * @file    events.cpp
 * @author  SO
 * @version v2.1.0
 * @date    16-June-2021
 * @brief   The system events of the oBank.
 ******************************************************************************
 */

// === Includes ===
#include "events.h"

// === Functions ===

/**
 * @brief Check whether the timeout event is triggered with the current time.
 *
 * When the event is triggered, the event is automatically reset when calling
 * this function!
 *
 * @param current_time The current time in milliseconds.
 * @return Returns true when the event is triggered.
 */
bool Event::Timeout::is_triggered(std::chrono::milliseconds current_time)
{
    // Check for timeout
    if (current_time - last_triggered >= timeout)
    {
        // remember the last triggered time and return
        last_triggered = current_time;
        return true;
    }
    return false;
};

/**
 * @brief Reset the last time the event was triggered.
 *
 * @param current_time The current time in milliseconds.
 */
void Event::Timeout::reset(std::chrono::milliseconds current_time)
{
    last_triggered = current_time;
};

/**
 * @brief Check whether the button was pressed.
 *
 * This function can only detect changes in the input
 * state and has to be called at least 2 time to detect
 * an edge.
 *
 * @return Returns true when the button event was triggered since the last call.
 * @attention Multiple Events cannot use the same IO Pin since every call to this
 * function updates the old pin state. Other edges in the same Pin are shadowed
 * by using multiple events with the same Pin!
 */
bool Event::Button::is_triggered(void)
{
    this->button->read_edge();
    bool is_rising = this->button->rising_edge();
    bool is_falling = this->button->falling_edge();

    switch (this->trigger_edge)
    {
    case GPIO::Edge::Rising:
        return is_rising;
    case GPIO::Edge::Falling:
        return is_falling;
    default:
        return false;
    }
};