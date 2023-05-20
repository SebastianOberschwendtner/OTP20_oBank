#ifndef SYSTEM_H_
#define SYSTEM_H_
// === Includes ===
#include "state_machine.h"
#include "events.h"

namespace System
{
    class Events
    {
    private:
        Event::Timeout user_timeout;
        Event::Timeout input_timeout;
        Event::Button user_button;

    public:
        // delete the default constructor
        Events() = delete;

        /**
         * @brief Construct a new Events object
         * 
         * @param timeout The timeout in milliseconds to go to sleep.
         * @param holdtime The timeout in milliseconds the button has to be pressed to trigger event.
         * @param input_button The button to be used for input.
         */
        Events(std::chrono::milliseconds timeout,
               std::chrono::milliseconds holdtime,
               GPIO::PIN &input_button)
            : user_timeout{timeout}
            , input_timeout{holdtime}
            , user_button{input_button, GPIO::Edge::Rising}
            {};

        // Process all events
        auto get_event(std::chrono::milliseconds current_time) -> etl::state_chart_traits::event_id_t 
        {
            // return the event ID when it is triggered
            if (user_timeout.is_triggered(current_time))
                return Event_ID::User_Timeout;
            if (user_button.is_triggered())
                return Event_ID::Button_Pressed;
            if (input_timeout.is_triggered(current_time))
                return Event_ID::Input_Timeout;

            // return an always event of no other event occurred
            return System::Event_ID::Always;
        };

        // Reset all timeouts
        void reset_timeouts(std::chrono::milliseconds current_time)
        {
            user_timeout.reset(current_time);
            input_timeout.reset(current_time);
        };
    };
}; // namespace System

#endif