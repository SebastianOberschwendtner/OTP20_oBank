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

namespace System
{
    // === IDs ===
    // => Important: The IDs have to be inside a class/struct
    // so that the template argument deduction can separate
    // the types of the IDs to have separate types.

    // *** States ***
    struct State_ID
    {
        enum 
        {
            Initialize,
            Info_Idle,
            Sleep,
            Wait_Input
        };
    };

    // *** Events ***
    struct Event_ID
    {
        enum
        {
            User_Timeout,
            Input_Timeout,
            Button_Pressed,
            Always
        };
    };

    // === Actions ===
    // Declare a class with the actions of the state machine
    struct Actions
    {
        // *** Actions *** 
        void Initialize(void);
        void Sleep(void);
        void Wake_Up(void);
        void Handle_User_Input(void);
        void Reset_Timeouts(void);
        bool not_charging(void);
    };

    // === State Table ===
    using state = etl::state_chart_traits::state<Actions>;
    constexpr state StateTable[] =
    {
        // *** States ***
        //   |--------------------|-------------------------|-------------------|
        //   | ID                 | Entry Action            | Exit Action       |
        //   |--------------------|-------------------------|-------------------|
        state(State_ID::Initialize, &Actions::Initialize    , nullptr          ),
        state(State_ID::Info_Idle , &Actions::Reset_Timeouts, nullptr          ),
        state(State_ID::Sleep     , &Actions::Sleep         , &Actions::Wake_Up),
        state(State_ID::Wait_Input, &Actions::Reset_Timeouts, nullptr          )
    };  

    // === Transition Table ===
    using transition = etl::state_chart_traits::transition<Actions>;
    constexpr transition TransitionTable[] =
    {
// *** Transitions ***
//        |--------------------|-------------------------|---------------------|----------------------------|-----------------------|
//        | Source State       | Event                   | Target State        | Action                     | Guard                 |
//        |--------------------|-------------------------|---------------------|----------------------------|-----------------------|
transition(State_ID::Initialize, Event_ID::Always        , State_ID::Info_Idle),
transition(State_ID::Info_Idle , Event_ID::User_Timeout  , State_ID::Sleep     , nullptr                    , &Actions::not_charging),
transition(State_ID::Sleep     , Event_ID::Always        , State_ID::Info_Idle),
transition(State_ID::Info_Idle , Event_ID::Button_Pressed, State_ID::Wait_Input),
transition(State_ID::Wait_Input, Event_ID::Input_Timeout , State_ID::Info_Idle , &Actions::Handle_User_Input)
    };
}; // namespace System

#endif