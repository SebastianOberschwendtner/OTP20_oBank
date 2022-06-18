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

#ifndef GUI_H_
#define GUI_H_

// === Includes ===
#include "graphics.h"

namespace GUI
{
    // === Functions ===
    unsigned char* get_data_pointer(void);
    void initialize_canvas(void);
    void draw_main_info(unsigned int voltage, signed int current);
    void draw_state_info(bool output_enabled, bool is_charging);
};
#endif