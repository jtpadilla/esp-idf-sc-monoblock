#pragma once

#include <memory>

#include "LorawanActivator.h"
#include "LorawanListener.h"
#include "LorawanClient.h"
#include "Task.h"
#include "Semaphore.h"

#include "CounterStatistics.h"

namespace example::lorawan
{

    class ExampleLorawanTask final: public genielink::lorawan::ILorawanActivator, public genielink::lorawan::ILorawanListener, public genielink::os::Task {

        public:
            ExampleLorawanTask();

            // implements -> genielink::lorawan::IlorawanActivator
            virtual void lorawanActivate(std::unique_ptr<genielink::lorawan::ILorawanClient> lorawanClient);

            // implements -> genielink::lorawan::ILorawanListener
            void lorawanMessageReceived(const uint8_t* payload, size_t length, port_t port);

            // implements -> genielink::os::Task
            void run(void* data);

        private:
            bool configureDevice(int id);
            bool receivedConfirmedCounter(int counter);
            bool receivedUnconfirmedCounter(int counter);

            // Controla el paso de valores entre las dos tares
            genielink::os::Semaphore lorawanClientAccess;

            // Valores y referencias escritor por la tarea de eventos y leidos por la tarea principal.
            std::unique_ptr<genielink::lorawan::ILorawanClient> lorawanClient;
            uint32_t deviceId;
            bool configured;

            // Estadisticas
            CounterStatistics counterStatistics;

    };

}