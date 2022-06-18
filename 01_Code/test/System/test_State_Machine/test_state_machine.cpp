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
 * @file    test_state_machine.cpp
 * @author  SO
 * @version v2.1.0
 * @date    07-June-2022
 * @brief   Unit tests for the State Machine of the oBank.
 ******************************************************************************
 */

// === Includes ===
#include "unity.h"
#include "state_machine.h"
#include "mock.h"
using namespace System;

// === Fixtures ===

// Provide the implementation of the actions
void Actions::Initialize(void) { return; };
void Actions::Sleep(void) { return; };
void Actions::Wake_Up(void) { return; };
void Actions::Handle_User_Input(void) { return; };
void Actions::Reset_Timeouts(void) { return; };
bool System::Actions::not_charging(void) { return true; };

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
Actions actions;
etl::state_chart_ct<
    Actions, actions,
    TransitionTable, std::size(TransitionTable),
    StateTable, std::size(StateTable),
    State_ID::Info_Idle>
    state_machine;

// === Tests ===

void test_available_states(void)
{
    TEST_ASSERT_EQUAL(State_ID::Initialize, State_ID::Initialize);
    TEST_ASSERT_EQUAL(State_ID::Info_Idle, State_ID::Info_Idle);
    TEST_ASSERT_EQUAL(State_ID::Sleep, State_ID::Sleep);
    TEST_ASSERT_EQUAL(State_ID::Wait_Input, State_ID::Wait_Input);
};

void test_available_events(void)
{
    TEST_ASSERT_EQUAL(Event_ID::User_Timeout, Event_ID::User_Timeout);
    TEST_ASSERT_EQUAL(Event_ID::Input_Timeout, Event_ID::Input_Timeout);
    TEST_ASSERT_EQUAL(Event_ID::Button_Pressed, Event_ID::Button_Pressed);
    TEST_ASSERT_EQUAL(Event_ID::Always, Event_ID::Always);
};

void test_state_table(void)
{
    TEST_ASSERT_EQUAL(State_ID::Initialize, StateTable[0].state_id);
    TEST_ASSERT_EQUAL(State_ID::Info_Idle, StateTable[1].state_id);
    TEST_ASSERT_EQUAL(State_ID::Sleep, StateTable[2].state_id);
    TEST_ASSERT_EQUAL(State_ID::Wait_Input, StateTable[3].state_id);
};

void test_transition_table(void)
{
    // Transition Initialize -> Info_Idle
    TEST_ASSERT_EQUAL(Event_ID::Always, TransitionTable[0].event_id);
    TEST_ASSERT_EQUAL(State_ID::Initialize, TransitionTable[0].current_state_id);
    TEST_ASSERT_EQUAL(State_ID::Info_Idle, TransitionTable[0].next_state_id);

    // Transition: Info_Idle -> Sleep
    TEST_ASSERT_EQUAL(Event_ID::User_Timeout, TransitionTable[1].event_id);
    TEST_ASSERT_EQUAL(State_ID::Info_Idle, TransitionTable[1].current_state_id);
    TEST_ASSERT_EQUAL(State_ID::Sleep, TransitionTable[1].next_state_id);

    // Transition: Sleep -> Info_Idle
    TEST_ASSERT_EQUAL(Event_ID::Always, TransitionTable[2].event_id);
    TEST_ASSERT_EQUAL(State_ID::Sleep, TransitionTable[2].current_state_id);
    TEST_ASSERT_EQUAL(State_ID::Info_Idle, TransitionTable[2].next_state_id);

    // Transition: Info_Idle -> Wait_Input
    TEST_ASSERT_EQUAL(Event_ID::Button_Pressed, TransitionTable[3].event_id);
    TEST_ASSERT_EQUAL(State_ID::Info_Idle, TransitionTable[3].current_state_id);
    TEST_ASSERT_EQUAL(State_ID::Wait_Input, TransitionTable[3].next_state_id);

    // Transition: Wait_Input -> Info_Idle, with timeout
    TEST_ASSERT_EQUAL(Event_ID::Input_Timeout, TransitionTable[4].event_id);
    TEST_ASSERT_EQUAL(State_ID::Wait_Input, TransitionTable[4].current_state_id);
    TEST_ASSERT_EQUAL(State_ID::Info_Idle, TransitionTable[4].next_state_id);
};

// => The following test is not needed since it would test the library code 
// and not the oBank code. but it is a nice example how the etl::state_machine
// is used.
// void test_state_machine(void)
// {
//     // Start the state machine
//     state_machine.start();

//     // Initial state is Info_Idle
//     TEST_ASSERT_EQUAL(State_ID::Info_Idle, state_machine.get_state_id());
    
//     // Trigger first transition
//     state_machine.process_event(Event_ID::User_Timeout);
//     TEST_ASSERT_EQUAL(State_ID::Sleep, state_machine.get_state_id());
//     mock_enable_sleep.assert_called_once();
// };

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