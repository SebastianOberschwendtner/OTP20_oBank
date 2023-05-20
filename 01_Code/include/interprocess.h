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

#ifndef INTERPROCESS_H_
#define INTERPROCESS_H_

// === Includes ===
#include <etl/message.h>
#include <etl/message_packet.h>
#include <etl/queue.h>
#include "ipc.h"
#include "tasks.h"

// === Inter-process communication ===
namespace IPC
{
    // === Define the PIDs of the tasks ===
    enum PID : unsigned char
    {
        System = 0,
        Display = 1,
        BMS = 2, // Battery Management
        PD = 3,  // Power Delivery Controller
        GUI = 4  // GUI Interface
    };

    // === Define the commands ===
    namespace Command
    {
        /* Define the different command messages */

        /**
         * @brief Set a voltage property.
         *        The effect of this message depends on the receiver!
         */
        struct Voltage: public etl::message<0>
        {
            uint16_t voltage{0}; // Voltage in [mV] // NOLINT
            explicit Voltage(uint16_t voltage) : voltage(voltage) {};
        };

        /**
         * @brief Set a current property.
         *        The effect of this message depends on the receiver!
         */
        struct Current: public etl::message<1>
        {
            uint16_t current{0}; // Current in [mA] // NOLINT
            explicit Current(uint16_t current) : current(current) {};
        };

        /**
         * @brief Set the state of an output. (On/Off/Toggle)
         *        The effect of this message depends on the receiver!
         */
        struct Output: public etl::message<2>
        {
            enum class State
            {
                On,
                Off,
                Toggle
            };
            State state{State::Off}; // The state to set the output to // NOLINT
            explicit Output(State state) : state(state) {};
        };
    }; // namespace Command

    /**
     * @brief Base class for all command queues.
     * 
     * @tparam cmds The accepted commands of the queue.
     */
    template<typename ...cmds>
    class CommandQueue
    {
    public:
        /**
         * @brief Add a command to the command queue.
         * The valid commands are known at compile time.
         * @return Returns true if the command was added to the queue.
         */
        template <typename Command>
        auto push_command(Command command) -> bool
        { 
            if (this->command_queue.full()) {
                return false;
            }
            this->command_queue.emplace(command);
            return true;
        }

    protected:
        using queue = etl::queue<etl::message_packet<cmds...>, 5>;
        queue command_queue{};
    };


    // ===  Data interface to the BMS task ===
    class BMS_Interface: public CommandQueue<
        Command::Voltage,
        Command::Current,
        Command::Output>
    {
    public:
        /* Define public data access */
        [[nodiscard]] static auto get_battery_voltage() -> uint16_t;
        [[nodiscard]] static auto get_battery_current() -> int16_t;
        [[nodiscard]] static auto get_cell_voltage(unsigned char cell) -> uint16_t;
        [[nodiscard]] static auto get_remaining_capacity() -> uint16_t;
        [[nodiscard]] static auto get_soc() -> uint16_t;
        [[nodiscard]] static auto get_time2empty() -> uint32_t;
        [[nodiscard]] static auto get_time2full() -> uint32_t;
        [[nodiscard]] static auto is_charging() -> bool;
        [[nodiscard]] static auto outputs_enabled() -> bool;

        /* Simple blocking interface functions*/
        static void sleep();
        static void wake();

    private:
        friend void ::Task_BMS(); // Allow the BMS task to access the command queue
    };

    // ===  Data interface to the PD task ===
    class PD_Interface: public CommandQueue<Command::Output>
    {
    public:
        /* Define public data access */
        [[nodiscard]] static auto get_voltage() -> uint16_t;
        [[nodiscard]] static auto get_current() -> uint16_t;

        /* Simple blocking interface functions*/
        static void sleep();
        static void wake();

    private:
        friend void ::Task_PD(); // Allow the PD task to access the command queue
    };

    // ===  Data interface to the GUI task ===
    struct Display_Interface
    {
        static void sleep();
        static void wake();
        static void next_page();
    };
}; // namespace IPC
#endif
