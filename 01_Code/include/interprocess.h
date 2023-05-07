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
    }; // namespace Command

    // ===  Data interface to the BMS task ===
    class BMS_Interface
    {
    public:
        /* Define the accepted command types */
        using commands = etl::message_packet<Command::Voltage, Command::Current>;

        /* Define public data access */
        [[nodiscard]] unsigned int get_battery_voltage() const;
        [[nodiscard]] signed int get_battery_current() const;
        [[nodiscard]] unsigned int get_cell_voltage(unsigned char cell) const;
        [[nodiscard]] unsigned int get_remaining_capacity() const;
        [[nodiscard]] unsigned int get_soc() const;
        [[nodiscard]] unsigned int get_time2empty() const;
        [[nodiscard]] unsigned int get_time2full() const;
        [[nodiscard]] bool is_charging() const;

        /* Simple blocking interface functions*/
        void sleep();
        void wake();

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
    private:
        friend void ::Task_BMS(); // Allow the BMS task to access the command queue
        etl::queue<commands, 10> command_queue{};
    };

    // ===  Data interface to the PD task ===
    struct PD_Interface
    {
        void sleep();
        void wake();
        [[nodiscard]] static auto get_voltage() -> uint16_t;
        [[nodiscard]] static auto get_current() -> uint16_t;
    };

    // ===  Data interface to the GUI task ===
    struct Display_Interface
    {
        void sleep();
        void wake();
        void next_page();
    };
}; // namespace IPC
#endif
