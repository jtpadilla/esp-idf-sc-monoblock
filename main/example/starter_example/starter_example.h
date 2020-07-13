#pragma once

#include "smooth/core/Application.h"
#include <iostream>

namespace starter_example
{
    class App : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;
    };
}
