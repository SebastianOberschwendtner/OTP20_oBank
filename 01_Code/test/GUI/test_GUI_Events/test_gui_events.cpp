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
 * @file    test_gui_events.cpp
 * @author  SO
 * @version v2.1.0
 * @date    18-June-2022
 * @brief   Unit tests for the GUI Events of the oBank.
 ******************************************************************************
 */

// === Includes ===
#include "unity.h"
#include "gui_events.h"
#include "mock.h"

// === Fixtures ===

// Provide the implementation of the actions
void GUI::Actions::Draw_Main_Info(void) { return; };
void GUI::Actions::Draw_Status_Info(void) { return; };
void GUI::Actions::Clear_Buffer(void) { return; };

// *** Setup and Teardown functions ***
void setUp(void)
{
    // set stuff up here
};

void tearDown(void){
    // clean stuff up here
};

// === UUT ===
// Declare the unit under test

// === Tests ===

void test_event_next_page(void)
{
    // Create the event
    Event::Next_Page event;

    // Event should not be triggered
    TEST_ASSERT_FALSE(event.is_triggered()); 

    // Next page can be manually triggered
    event.trigger();
    TEST_ASSERT_TRUE(event.is_triggered());

    // The read of the event should reset it
    TEST_ASSERT_FALSE(event.is_triggered());
};

// === Main ===

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_event_next_page);
    return UNITY_END();
};

