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
 * @file    test_gui_state_machine.cpp
 * @author  SO
 * @version v3.0.0
 * @date    18-June-2022
 * @brief   Unit tests for the GUI State Machine of the oBank.
 ******************************************************************************
 */

// === Includes ===
#include "unity.h"
#include "gui_state_machine.h"
#include "mock.h"

// === Fixtures ===

// Provide the implementation of the actions
void GUI::Actions::Draw_Main_Info(void) { return; };
void GUI::Actions::Draw_Status_Info(void) { return; };
void GUI::Actions::Draw_Cell_Info(void) { return; };
void GUI::Actions::Draw_SOC_Info(void) { return; };
void GUI::Actions::Draw_Time_Info(void) { return; };
void GUI::Actions::Draw_PD_Info(void) { return; };
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

void test_available_states(void)
{
    TEST_ASSERT_EQUAL(GUI::State_ID::Main_Info, GUI::State_ID::Main_Info);
    TEST_ASSERT_EQUAL(GUI::State_ID::Status_Info, GUI::State_ID::Status_Info);
    TEST_ASSERT_EQUAL(GUI::State_ID::Cell_Info, GUI::State_ID::Cell_Info);
    TEST_ASSERT_EQUAL(GUI::State_ID::SOC_Info, GUI::State_ID::SOC_Info);
    TEST_ASSERT_EQUAL(GUI::State_ID::Time_Info, GUI::State_ID::Time_Info);
    TEST_ASSERT_EQUAL(GUI::State_ID::PD_Info, GUI::State_ID::PD_Info);
};

void test_available_events(void)
{
    TEST_ASSERT_EQUAL(GUI::Event_ID::Always, GUI::Event_ID::Always);
    TEST_ASSERT_EQUAL(GUI::Event_ID::Next_Page, GUI::Event_ID::Next_Page);
};

void test_state_table(void)
{
    TEST_ASSERT_EQUAL(GUI::State_ID::Main_Info, GUI::StateTable[0].state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Status_Info, GUI::StateTable[1].state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Cell_Info, GUI::StateTable[2].state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::SOC_Info, GUI::StateTable[3].state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Time_Info, GUI::StateTable[4].state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::PD_Info, GUI::StateTable[5].state_id);
};

void test_transition_table(void)
{
    // Draw_Main_Info -> Draw_Main_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Always, GUI::TransitionTable[0].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Main_Info, GUI::TransitionTable[0].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Main_Info, GUI::TransitionTable[0].next_state_id);

    // Draw_Main_Info -> Draw_Status_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Next_Page, GUI::TransitionTable[1].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Main_Info, GUI::TransitionTable[1].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Status_Info, GUI::TransitionTable[1].next_state_id);

    // Draw_Status_Info -> Draw_Status_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Always, GUI::TransitionTable[2].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Status_Info, GUI::TransitionTable[2].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Status_Info, GUI::TransitionTable[2].next_state_id);

    // Draw_Status_Info -> Draw_Cell_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Next_Page, GUI::TransitionTable[3].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Status_Info, GUI::TransitionTable[3].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Cell_Info, GUI::TransitionTable[3].next_state_id);

    // Draw_Cell_Info -> Draw_Cell_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Always, GUI::TransitionTable[4].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Cell_Info, GUI::TransitionTable[4].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Cell_Info, GUI::TransitionTable[4].next_state_id);

    // Draw_Cell_Info -> Draw_SOC_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Next_Page, GUI::TransitionTable[5].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Cell_Info, GUI::TransitionTable[5].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::SOC_Info, GUI::TransitionTable[5].next_state_id);

    // Draw_SOC_Info -> Draw_SOC_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Always, GUI::TransitionTable[6].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::SOC_Info, GUI::TransitionTable[6].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::SOC_Info, GUI::TransitionTable[6].next_state_id);

    // Draw_SOC_Info -> Draw_Time_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Next_Page, GUI::TransitionTable[7].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::SOC_Info, GUI::TransitionTable[7].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Time_Info, GUI::TransitionTable[7].next_state_id);

    // Draw_Time_Info -> Draw_Time_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Always, GUI::TransitionTable[8].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Time_Info, GUI::TransitionTable[8].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Time_Info, GUI::TransitionTable[8].next_state_id);

    // Draw_Time_Info -> Draw_PD_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Next_Page, GUI::TransitionTable[9].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Time_Info, GUI::TransitionTable[9].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::PD_Info, GUI::TransitionTable[9].next_state_id);

    // PD_Time_Info -> Draw_PD_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Always, GUI::TransitionTable[10].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::PD_Info, GUI::TransitionTable[10].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::PD_Info, GUI::TransitionTable[10].next_state_id);

    // Draw_PD_Info -> Draw_Main_Info
    TEST_ASSERT_EQUAL(GUI::Event_ID::Next_Page, GUI::TransitionTable[11].event_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::PD_Info, GUI::TransitionTable[11].current_state_id);
    TEST_ASSERT_EQUAL(GUI::State_ID::Main_Info, GUI::TransitionTable[11].next_state_id);
};

// === Main ===

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_available_states);
    RUN_TEST(test_available_events);
    RUN_TEST(test_state_table);
    RUN_TEST(test_transition_table);
    return UNITY_END();
};
