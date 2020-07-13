#pragma once

#include <memory>

#include "LorawanClient.h"

namespace genielink::lorawan
{
    
    class ILorawanActivator
    {
        public:
            virtual void lorawanActivate(std::unique_ptr<ILorawanClient> lorawanClient) = 0;

    };

}
