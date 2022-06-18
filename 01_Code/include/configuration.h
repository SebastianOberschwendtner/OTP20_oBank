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
 * ==============================================================================
 * This file is intended to configure the oBank and set user specific parameters.
 * This file can be modified to fit your needs. 
 * ==============================================================================
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
#include <chrono>
using namespace std::chrono_literals;

namespace detail
{
    // === Define the version of the oBank ===
    constexpr char __version__[] = "v2.1.0";
};

namespace User
{
    // === Timeouts ===
    namespace Timeout
    {
        // === Timeout for the user interface ===
        /**
         * @brief The timeout for the user interface in milli seconds.
         * @fixed: Add strong time type. 
         */
        constexpr auto Display = 10s;

        /**
         * @brief The timeout for detecting a button hold.
         */
        constexpr auto Button_Hold = 250ms;
    };

    // === Power Parameters ===
    namespace Power
    {
        // === Charging Parameters ===
        constexpr unsigned int Max_Charge_Current = 1000; // [mA]
    };
};

#endif