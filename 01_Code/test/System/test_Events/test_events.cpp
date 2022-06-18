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
 * @file    test_events.cpp
 * @author  SO
 * @version v2.1.0
 * @date    16-June-2022
 * @brief   Unit tests for the System Events of the oBank.
 ******************************************************************************
 */

// === Includes ===
#include "unity.h"
#include "events.h"
#include "mock.h"

// === Fixtures ===

// Provide the implementation of the actions
void System::Actions::Sleep(void) { return; };
void System::Actions::Wake_Up(void) { return; };
void System::Actions::Initialize(void) { return; };
void System::Actions::Handle_User_Input(void) { return; };
void System::Actions::Reset_Timeouts(void) { return; };
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

// === Tests ===

void test_event_timeout(void)
{
    // Arrange
    Event::Timeout event(300ms);
    std::chrono::milliseconds current_time{0};

    // Assert
    TEST_ASSERT_FALSE(event.is_triggered(current_time));

    // Update time and test whether event gets triggered
    current_time = 100ms;
    TEST_ASSERT_FALSE(event.is_triggered(current_time));

    // Update time and test whether event gets triggered
    current_time = 300ms;
    TEST_ASSERT_TRUE(event.is_triggered(current_time));
    // The readout should reset the event
    TEST_ASSERT_FALSE(event.is_triggered(current_time));

    // Update time and test whether event gets triggered
    current_time = 599ms;
    TEST_ASSERT_FALSE(event.is_triggered(current_time));

    // Update time and test whether event gets triggered
    current_time = 602ms;
    TEST_ASSERT_TRUE(event.is_triggered(current_time));
    // The readout should reset the event
    TEST_ASSERT_FALSE(event.is_triggered(current_time));

    // reset the last triggered time
    current_time = 800ms;
    event.reset(current_time);
    TEST_ASSERT_FALSE(event.is_triggered(current_time));
    current_time = 901ms;
    TEST_ASSERT_FALSE(event.is_triggered(current_time));
    current_time = 1100ms;
    TEST_ASSERT_TRUE(event.is_triggered(current_time));

};

void test_event_button(void)
{
    // Arrange
    GPIO::PIN button(GPIO::Port::A, 0, GPIO::Mode::Input);
    Event::Button event(button, GPIO::Edge::Rising);
    Event::Button event2( button, GPIO::Edge::Falling );

    // When the input is not triggered, the event should not be triggered
    GPIOA->registers_to_default();
    TEST_ASSERT_FALSE(event.is_triggered());
    TEST_ASSERT_FALSE(event.is_triggered());

    // When the input is triggered, the event should be triggered
    GPIOA->IDR = 0b0001;
    TEST_ASSERT_TRUE(event.is_triggered());
    // Reading the event should clear it
    TEST_ASSERT_FALSE(event.is_triggered());

    // Falling edge should not trigger the event
    GPIOA->IDR = 0b0000;
    TEST_ASSERT_FALSE(event.is_triggered());

    // test the event with falling edges
    TEST_ASSERT_FALSE(event2.is_triggered());
    GPIOA->IDR = 0b0001;
    TEST_ASSERT_FALSE(event2.is_triggered());
    GPIOA->IDR = 0b0000;
    TEST_ASSERT_TRUE(event2.is_triggered());
    TEST_ASSERT_FALSE(event2.is_triggered());
};

// === Main ===

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_event_timeout);
    RUN_TEST(test_event_button);
    return UNITY_END();
};
