#pragma once

#include <stdint.h>

#include "LorawanParameters.h"
#include "LorawanActivator.h"
#include "LorawanDevice.h"

namespace genielink::lorawan
{

    class LorawanLauncher 
    {

        public:
            static void launch(const LorawanDevice lorawanDevice, const LorawanParameters& lorawanParameters, ILorawanActivator& lorawanActivator, size_t retrySeconds = 10);

    };

}
