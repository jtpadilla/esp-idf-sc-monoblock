
#pragma once

#include <stdint.h>

#include "LorawanListener.h"

namespace genielink::lorawan
{

    enum class LorawanResponse
    {
        ErrorTransmissionFailed = -1,
        ErrorUnexpected = -10,
        SuccessfulTransmission = 1,
        SuccessfulReceive = 2
    };

    class ILorawanClient
    {

        public:
            virtual void installListener(ILorawanListener *lorawanListener) = 0;
            virtual void reset() = 0;
            virtual LorawanResponse transmitMessage(const uint8_t *payload, size_t length, port_t port, bool confirm = false) = 0;
            virtual void setRSSICal(int8_t rssiCal) = 0;

    };

}

