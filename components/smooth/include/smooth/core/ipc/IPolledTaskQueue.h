#pragma once

#include "ITaskEventQueue.h"

namespace smooth::core::ipc
{
    class QueueNotification;

    /// Una cola que es sondeada por la la tarea propietaria, en lugar de notificarse asi mismo la tarea.
    /// Como programador de aplicaciones, no debes llamar a ninguno de estos métodos.
    class IPolledTaskQueue
        : public ITaskEventQueue
    {
        public:
            // Sondea la cola, y si hay datos disponibles, senyalara a la tarea propietaria.
            virtual void poll() = 0;
    };
}
