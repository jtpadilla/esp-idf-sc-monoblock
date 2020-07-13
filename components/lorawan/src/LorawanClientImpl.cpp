
#include <memory>

#include "esp_log.h"

#include "LorawanDevice.h"
#include "LorawanClientImpl.h"
#include "LorawanParameters.h"
#include "driver/LorawanDriver.h"

namespace genielink::lorawan
{

    LorawanClientImpl::LorawanClientImpl(const LorawanDevice lorawanDevice, const LorawanParameters& lorawanParameters):
        lorawanDriver{genielink::lorawan::driver::LorawanDriver::instantiate(lorawanDevice, lorawanParameters)}
    {
    }

    void LorawanClientImpl::installListener(ILorawanListener *lorawanListener) 
    {
        lorawanDriver->installListener(lorawanListener);
    }

    void LorawanClientImpl::reset() 
    {
        lorawanDriver->reset();
    }

    bool LorawanClientImpl::join() 
    {
        return lorawanDriver->join();
    }

    LorawanResponse LorawanClientImpl::transmitMessage(const uint8_t *payload, size_t length, port_t port, bool confirm) 
    {
        return lorawanDriver->transmitMessage(payload, length, port, confirm);
    }

    void LorawanClientImpl::setRSSICal(int8_t rssiCal) 
    {
        lorawanDriver->setRSSICal(rssiCal);
    }

}
