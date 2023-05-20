/**
 * OTP20 - oBank
 * Copyright (c) 2023 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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

#ifndef TASK_PD_H_
#define TASK_PD_H_

// === Includes ===
#include "interprocess.h"
#include "etl/message_router.h"

namespace PD
{
    /**
     * @brief Command handler for the BMS task.
     *        This class is used to receive commands from other tasks.
     * @details
     * The handler currently handles the following commands:
     * - Voltage
     * - Current
     */
    class Handler : public etl::message_router<Handler, IPC::Command::Output>
    {
    public:
        explicit Handler(const uint8_t router_id) : message_router(router_id) {};
        static void on_receive(const IPC::Command::Output &msg);
        void on_receive_unknown(const etl::imessage &msg) {};
    };
}; // namespace PD

#endif /* TASK_PD_H_ */
