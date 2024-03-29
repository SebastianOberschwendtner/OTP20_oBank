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
 * @version v3.1.0
 * @date    17-June-2022
 * @brief   Defines the GUI pages for the display.
 ******************************************************************************
 */

// === Includes ===
#include "gui.h"
#include <string>

namespace
{
    // *** String buffer ***
    char line_string[24] = {0}; // NOLINT(*-avoid-c-arrays)
    // *** Display buffer ***
    Graphics::Buffer_BW<128, 32> GUI_Buffer;
} // namespace
// *** Display canvas object ***
Graphics::Canvas_BW canvas(GUI_Buffer.data.data(), GUI_Buffer.width_px, GUI_Buffer.height_px);

// === Functions ===
// NOLINTBEGIN(*-array-to-pointer-decay,*-easily-swappable-parameters,*-pro-type-vararg)
/**
 * @brief Get the pointer to the data buffer for sending
 * it to the display.
 *
 * @return Pointer to the data buffer.
 */
auto GUI::get_data_pointer() -> unsigned char *
{
    // Return the pointer to the data buffer
    return GUI_Buffer.data.data();
};

/**
 * @brief Initialize the canvas and the display buffer.
 */
void GUI::initialize_canvas()
{
    // Empty the canvas
    canvas.fill(Graphics::Black);

    // Write the first content in canvas
    canvas.set_font(Font::_16px::Default);
};

/**
 * @brief Draw the main info page on the canvas.
 *
 * @param voltage The measured battery voltage.
 * @param current The measured battery current.
 */
void GUI::draw_main_info(
    const uint16_t voltage,
    const int16_t current)
{
    // Draw the voltage information
    canvas.set_cursor(0, 0);
    std::sprintf(line_string, "U: %d.%02d  V", voltage / 1000, (voltage / 10) % 100);
    canvas.write(line_string, 10);

    // Draw the current information
    canvas.set_cursor(0, 1);
    std::sprintf(line_string, "I:%+5d mA", current);
    canvas.write(line_string, 10);
};

/**
 * @brief Draw the status page on the canvas.
 *
 * @param output_enabled Whether the output is enabled or not.
 * @param is_charging Whether the battery is charging or not.
 */
void GUI::draw_state_info(
    const bool output_enabled,
    const bool is_charging)
{
    // Draw the output enabled information
    canvas.set_cursor(0, 0);
    if (output_enabled)
        canvas.write("5V:    ON ", 10);
    else
        canvas.write("5V:    OFF", 10);

    // Draw the charging information
    canvas.set_cursor(0, 1);
    if (is_charging)
        canvas.write("Input: ON ", 10);
    else
        canvas.write("Input: OFF", 10);
};

/**
 * @brief Draw the cell info page on the canvas.
 *
 * @param cell_1 The measured voltage of cell 1 in [mV]
 * @param cell_2 The measured voltage of cell 2 in [mV]
 */
void GUI::draw_cell_info(
    const uint16_t cell_1,
    const uint16_t cell_2)
{
    // Draw first cell voltage
    canvas.set_cursor(0, 0);
    std::sprintf(line_string, "C1: %d.%03dV", cell_1 / 1000, cell_1 - (cell_1 / 1000) * 1000);
    canvas.write(line_string, 10);

    // Draw second cell voltage
    canvas.set_cursor(0, 1);
    std::sprintf(line_string, "C2: %d.%03dV", cell_2 / 1000, cell_2 - (cell_2 / 1000) * 1000);
    canvas.write(line_string, 10);
};

/**
 * @brief Draw the state of charge information on the canvas.
 *
 * @param capacity The remaining capacity of the battery in [mAh].
 * @param soc The state of charge of the battery in [%].
 * @todo Adjust the formatting of the capacity and soc.
 */
void GUI::draw_soc_info(const uint16_t capacity, const uint16_t soc)
{
    // Draw the capacity information
    canvas.set_cursor(0, 0);
    std::sprintf(line_string, "Cap:%2d.%dAh", capacity / 1000, (capacity % 1000) / 100);
    canvas.write(line_string, 10);

    // Draw the SOC information
    canvas.set_cursor(0, 1);
    std::sprintf(line_string, "SOC: %3d %%", soc);
    canvas.write(line_string, 10);
};

/**
 * @brief Draw the time information on the canvas.
 *
 * @param time2empty The time until the battery is empty in [s].
 * @param time2full The time until the battery is full in [s].
 * @todo Adjust the formatting of the time.
 */
void GUI::draw_time_info(const uint32_t time2empty, const uint32_t time2full)
{
    // Draw the time to empty information
    canvas.set_cursor(0, 0);
    std::sprintf(line_string, "TTE:%3ld.%ldh", time2empty / 3600, (time2empty % 3600) / 360);
    canvas.write(line_string, 10);

    // Draw the time to full information
    canvas.set_cursor(0, 1);
    std::sprintf(line_string, "TTF:%3ld.%ldh", time2full / 3600, (time2full % 3600) / 360);
    canvas.write(line_string, 10);
};

/**
 * @brief Clear the canvas for changing the page.
 */
void GUI::clear_canvas()
{
    // Empty the canvas
    GUI_Buffer.data.fill(0);
};

// NOLINTEND(*-array-to-pointer-decay,*-easily-swappable-parameters,*-pro-type-vararg)