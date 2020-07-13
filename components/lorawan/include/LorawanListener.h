#pragma once

#include <stdint.h>

using port_t = uint8_t;

namespace genielink::lorawan
{
    
    class ILorawanListener
    {
        public:
            virtual void lorawanMessageReceived(const uint8_t* payload, size_t length, port_t port) = 0;

    };

}
