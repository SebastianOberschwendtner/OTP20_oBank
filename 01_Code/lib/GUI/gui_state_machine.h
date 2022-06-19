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

#ifndef GUI_STATE_MACHINE_H_
#define GUI_STATE_MACHINE_H_

// === Includes ===
#include "etl/state_chart.h"

namespace GUI
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
            Main_Info, Status_Info, Cell_Info, SOC_Info, Time_Info
        };
    };

    // *** Events ***
    struct Event_ID
    {
        enum 
        {
            Always, Next_Page
        };
    };

    // === Actions ===
    // Declare a class with the actions of the state machine
    struct Actions
    {
        // *** Actions *** 
        void Draw_Main_Info(void);
        void Draw_Status_Info(void);
        void Draw_Cell_Info(void);
        void Draw_SOC_Info(void);
        void Draw_Time_Info(void);
        void Clear_Buffer(void);
    };

    // === State Table ===
    using state = etl::state_chart_traits::state<Actions>;
    constexpr state StateTable[] =
    {
        // *** States ***
        //   |---------------------|-------------------------|-------------------|
        //   | ID                  | Entry Action            | Exit Action       |
        //   |---------------------|-------------------------|-------------------|
        state(State_ID::Main_Info  , nullptr                 , nullptr          ),
        state(State_ID::Status_Info, nullptr                 , nullptr          ),
        state(State_ID::Cell_Info  , nullptr                 , nullptr          ),
        state(State_ID::SOC_Info   , nullptr                 , nullptr          ),
        state(State_ID::Time_Info  , nullptr                 , nullptr          )
    };

    // === Transition Table ===
    using transition = etl::state_chart_traits::transition<Actions>;
    constexpr transition TransitionTable[] =
    {
// *** Transitions ***
//        |---------------------|-------------------------|----------------------|----------------------------|-----------------------|
//        | Source State        | Event                   | Target State         | Action                     | Guard                 |
//        |---------------------|-------------------------|----------------------|----------------------------|-----------------------|
transition(State_ID::Main_Info  , Event_ID::Always        , State_ID::Main_Info  , &Actions::Draw_Main_Info   , nullptr),
transition(State_ID::Main_Info  , Event_ID::Next_Page     , State_ID::Status_Info, &Actions::Clear_Buffer     , nullptr),
transition(State_ID::Status_Info, Event_ID::Always        , State_ID::Status_Info, &Actions::Draw_Status_Info , nullptr),
transition(State_ID::Status_Info, Event_ID::Next_Page     , State_ID::Cell_Info  , &Actions::Clear_Buffer     , nullptr),
transition(State_ID::Cell_Info  , Event_ID::Always        , State_ID::Cell_Info  , &Actions::Draw_Cell_Info   , nullptr),
transition(State_ID::Cell_Info  , Event_ID::Next_Page     , State_ID::SOC_Info   , &Actions::Clear_Buffer     , nullptr),
transition(State_ID::SOC_Info   , Event_ID::Always        , State_ID::SOC_Info   , &Actions::Draw_SOC_Info    , nullptr),
transition(State_ID::SOC_Info   , Event_ID::Next_Page     , State_ID::Time_Info  , &Actions::Clear_Buffer     , nullptr),
transition(State_ID::Time_Info  , Event_ID::Always        , State_ID::Time_Info  , &Actions::Draw_Time_Info   , nullptr),
transition(State_ID::Time_Info  , Event_ID::Next_Page     , State_ID::Main_Info  , &Actions::Clear_Buffer     , nullptr)
    };
};
#endif