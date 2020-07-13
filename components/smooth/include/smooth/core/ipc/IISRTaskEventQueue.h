#pragma once

#include "esp_attr.h"
#define SMOOTH_MEM_ATTR IRAM_ATTR

#include <type_traits>

namespace smooth::core::ipc
{
    /// Interface for ISR-safe task queue, see ISRTaskEventQueue fore more details.
    /// \tparam DataType The type of data to send from the ISR.
    template<typename DataType>
    class IISRTaskEventQueue
    {
        static_assert(std::is_default_constructible<DataType>::value, "DataType must be default-constructible");
        static_assert(std::is_trivial<DataType>::value, "DataType must be a trivial type");
        public:
            virtual ~IISRTaskEventQueue() = default;

            /// When called from an ISR, signals the queue to create an event.
            /// \param data The data to attach to the signal.
            SMOOTH_MEM_ATTR virtual void signal(const DataType& data) = 0;
    };
}
