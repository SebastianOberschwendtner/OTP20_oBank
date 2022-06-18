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
 * @file    gui.cpp
 * @author  SO
 * @version v2.1.0
 * @date    17-June-2022
 * @brief   Defines the GUI pages for the display.
 ******************************************************************************
 */

// === Includes ===
#include "gui.h"
#include <string>

// *** String buffer ***
static char line_string[24] = {0};
// *** Display buffer ***
static Graphics::Buffer_BW<128, 32> GUI_Buffer;
// *** Display canvas object ***
static Graphics::Canvas_BW canvas(GUI_Buffer.data.data(), GUI_Buffer.width_px, GUI_Buffer.height_px);

// === Functions ===

/**
 * @brief Get the pointer to the data buffer for sending
 * it to the display.
 *
 * @return Pointer to the data buffer.
 */
unsigned char *GUI::get_data_pointer(void)
{
    // Return the pointer to the data buffer
    return GUI_Buffer.data.data();
};

/**
 * @brief Initialize the canvas and the display buffer.
 */
void GUI::initialize_canvas(void)
{
    // Empty the canvas
    canvas.fill(Graphics::Black);

    // Write the first content in canvas
    canvas.set_fontsize(Font::Size::Normal);
};

/**
 * @brief Draw the main info page on the canvas.
 *
 * @param voltage The measured battery voltage.
 * @param current The measured battery current.
 */
void GUI::draw_main_info(
    unsigned int voltage,
    signed int current)
{
    // Draw the voltage information
    canvas.set_cursor(0, 0);
    std::sprintf(line_string, "U: %d.%02d  V", voltage / 1000, (voltage / 10) % 100);
    canvas.add_string(line_string);

    // Draw the current information
    canvas.set_cursor(0, 1);
    std::sprintf(line_string, "I:%+5d mA", current);
    canvas.add_string(line_string);
};

void GUI::draw_state_info(
    bool output_enabled,
    bool is_chargin)
{
    // Draw the output enabled information
    canvas.set_cursor(0, 0);
    if (output_enabled)
        canvas.add_string("5V enabled");
    else
        canvas.add_string("5V disabled");

    // Draw the charging information
    canvas.set_cursor(0, 1);
    if (is_chargin)
        canvas.add_string("Charging");
    else
        canvas.add_string("Not Charging");
};