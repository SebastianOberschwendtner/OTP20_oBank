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

#ifndef EVENTS_H_
#define EVENTS_H_

// === Includes ===
#include "state_machine.h"
#include "etl/optional.h"
#include "drivers.h"
#include <chrono>
using namespace std::chrono_literals;

namespace Event
{

    // === Events ===
    class Timeout
    {
    private:
        // *** Properties ***
        std::chrono::milliseconds const timeout;     // The timeout in milliseconds
        std::chrono::milliseconds last_triggered{0}; // The last time the event was triggered

    public:
        // *** Constructor ***
        // delete the default constructor
        Timeout() = delete;

        /**
         * @brief Construct a new Event_Timeout object
         *
         * @param timeout The timeout in milliseconds
         */
        Timeout(std::chrono::milliseconds timeout) : timeout(timeout) {};

        // *** Methods ***

        bool is_triggered(std::chrono::milliseconds current_time);
        void reset(std::chrono::milliseconds current_time);
    };

    class Button
    {
    private:
        // *** Properties ***
        GPIO::PIN *button;             // The button pin
        GPIO::Edge const trigger_edge; // The edge to trigger the event

    public:
        // *** Constructor ***
        // delete the default constructor
        Button() = delete;

        /**
         * @brief Construct a new Button object
         *
         * @param button The button to be used
         * @param edge The edge which triggers the event
         */
        Button(GPIO::PIN &button, GPIO::Edge edge) : button(&button), trigger_edge(edge){};

        // *** Methods ***

        bool is_triggered(void);
    };

};

    // === Functions ===

#endif