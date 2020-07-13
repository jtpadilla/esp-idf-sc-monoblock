
#include "esp_log.h"

#include "CounterStatistics.h"

namespace example::lorawan
{

    static const char* TAG = "Counters";

    CounterStatistics::CounterStatistics():
        confirmedEmpty {true}, confirmedCurrent {0}, confirmedValidCounter{0}, confirmedLaterCounter{0}, confirmedPreviousCounter{0},
        unconfirmedEmpty {true}, unconfirmedCurrent {0}, unconfirmedValidCounter{0}, unconfirmedLaterCounter{0}, unconfirmedPreviousCounter{0}
    {
    }

    void CounterStatistics::updateConfirmed(uint32_t counter) {

        if (confirmedEmpty) {
            confirmedEmpty = false;
            confirmedCurrent = counter;
            confirmedValidCounter++;
        } else {
            if (counter == (confirmedCurrent + 1)) {
                confirmedCurrent = counter;
                confirmedValidCounter++;
            } else if (counter > (confirmedCurrent + 1)) {
                confirmedCurrent = counter;
                confirmedLaterCounter++;
            } else {
                confirmedPreviousCounter++;
            }
        }

        displayCounters(true);

    }

    void CounterStatistics::updateUnconfirmed(uint32_t counter) {
        
        if (unconfirmedEmpty) {
            unconfirmedEmpty = false;
            unconfirmedCurrent = counter;
            unconfirmedValidCounter++;
        } else {
            if (counter == (unconfirmedCurrent + 1)) {
                unconfirmedCurrent = counter;
                unconfirmedValidCounter++;
            } else if (counter > (unconfirmedCurrent + 1)) {
                unconfirmedCurrent = counter;
                unconfirmedLaterCounter++;
            } else {
                unconfirmedPreviousCounter++;
            }
        }

        displayCounters(false);

    }

    void CounterStatistics::displayCounters(bool confirmed) {
        ESP_LOGI(TAG,  "%s[CONFIRMED: current=%d, valid=%d, later=%d, previous=%d]  %s[UNCONFIRMED: current=%d, valid=%d, later=%d, previous=%d]", 
            confirmed ? "***" : "   ",
            confirmedCurrent, confirmedValidCounter, confirmedLaterCounter, confirmedPreviousCounter,
            confirmed ? "   " : "***",
            unconfirmedCurrent, unconfirmedValidCounter, unconfirmedLaterCounter, unconfirmedPreviousCounter);
    }
    
}
