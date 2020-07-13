#include "smooth/core/timer/ElapsedTime.h"

using namespace std::chrono;

namespace smooth::core::timer
{

    std::chrono::microseconds ElapsedTime::get_running_time()
    {
        if (active)
        {
            // Calcula el nuevo tiempo transcurrido
            end_time = std::chrono::steady_clock::now();
            elapsed = end_time - start_time;
        }

        return duration_cast<microseconds>(elapsed);
    }

    std::chrono::microseconds ElapsedTime::get_running_time() const
    {
        steady_clock::duration local_elapsed{};

        if (active)
        {
            // Calcula el nuevo tiempo transcurrido
            local_elapsed = steady_clock::now() - start_time;
        }

        return duration_cast<microseconds>(local_elapsed);
    }
    
}
