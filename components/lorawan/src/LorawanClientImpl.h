#pragma once

#include <memory>

#include "LorawanClient.h"
#include "LorawanParameters.h"
#include "LorawanListener.h"
#include "driver/LorawanDriver.h"

namespace genielink::lorawan
{

    class LorawanClientImpl: public ILorawanClient
    {

        public:

            LorawanClientImpl(const LorawanDevice lorawanDevice, const LorawanParameters& lorawanParameters);

            virtual void installListener(ILorawanListener *lorawanListener);
            virtual void reset();
            virtual LorawanResponse transmitMessage(const uint8_t *payload, size_t length, port_t port, bool confirm = false);
            virtual void setRSSICal(int8_t rssiCal);

            // No lo tiene ILoraClient
            bool join();

        private:
            std::unique_ptr<genielink::lorawan::driver::LorawanDriver> lorawanDriver;

    };

}