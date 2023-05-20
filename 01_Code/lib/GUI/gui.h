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
#include <chrono>
#include <cstdint>
#include "graphics.h"
#include "gui_state_machine.h"
#include "gui_events.h"

namespace GUI
{
    // === Events ===
    class Events
    {
    private:

    public:
        Event::Next_Page next_page;

        // *** Methods ***
        auto get_event() -> etl::state_chart_traits::event_id_t 
        {
            // Check for triggered events and return their ID
            if (next_page.is_triggered())
                return GUI::Event_ID::Next_Page;

            // No event triggered
            return GUI::Event_ID::Always;
        }
    };

    // === Functions ===
    auto get_data_pointer() -> unsigned char*;
    void initialize_canvas();
    void draw_main_info(uint16_t voltage, int16_t current);
    void draw_state_info(bool output_enabled, bool is_charging);
    void draw_cell_info(uint16_t cell_1, uint16_t cell_2);
    void draw_soc_info(uint16_t capacity, uint16_t soc);
    void draw_time_info(uint32_t time2empty, uint32_t time2full);
    void clear_canvas();
}; // namespace GUI
#endif