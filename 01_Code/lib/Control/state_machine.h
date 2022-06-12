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

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

// === Includes ===
#include "etl/state_chart.h"

// === IDs ===
// => Important: The IDs have to be inside a class/struct
// so that the template argument deduction can separate
// the types of the IDs to have separate types.

// *** States ***
struct State_ID
{
    enum 
    {
        Info_Idle,
        Sleep
    };
};

// *** Events ***
struct Event_ID
{
    enum
    {
        User_Timeout,
        Button_Pressed
    };
};

// === Actions ===
// Declare a class with the actions of the state machine
struct Actions
{
    // *** Actions *** 
    void Sleep(void);
    void Wake_Up(void);
};

// === State Table ===
using state = etl::state_chart_traits::state<Actions>;
constexpr state StateTable[] =
{
    // *** States ***
    //   |-------------------|----------------|-------------------|
    //   | ID                | Entry Action   | Exit Action       |
    //   |-------------------|----------------|-------------------|
    state(State_ID::Info_Idle, nullptr        , nullptr          ),
    state(State_ID::Sleep    , &Actions::Sleep, &Actions::Wake_Up)
};  

// === Transition Table ===
using transition = etl::state_chart_traits::transition<Actions>;
constexpr transition TransitionTable[] =
{
    // *** Transitions ***
    //        |-------------------|-------------------------|--------------------|--------|
    //        | Source State      | Event                   | Target State       | Action |
    //        |-------------------|-------------------------|--------------------|--------|
    transition(State_ID::Info_Idle, Event_ID::User_Timeout  , State_ID::Sleep    ),
    transition(State_ID::Sleep    , Event_ID::Button_Pressed, State_ID::Info_Idle)
};

#endif