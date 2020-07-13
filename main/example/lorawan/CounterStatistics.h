#pragma once

#include <string>
#include <cstdint>

namespace example::lorawan
{

    class CounterStatistics {

        public:
            CounterStatistics();
            void updateConfirmed(uint32_t counter);
            void updateUnconfirmed(uint32_t counter);

        private:

            void displayCounters(bool confirmed);

            bool confirmedEmpty;
            uint32_t confirmedCurrent;
            uint32_t confirmedValidCounter;
            uint32_t confirmedLaterCounter;
            uint32_t confirmedPreviousCounter;

            bool unconfirmedEmpty;
            uint32_t unconfirmedCurrent;
            uint32_t unconfirmedValidCounter;
            uint32_t unconfirmedLaterCounter;
            uint32_t unconfirmedPreviousCounter;

    };

}