#pragma once

namespace smooth::core::ipc
{
    class QueueNotification;

    /// Interface comun para TaskEventQueue
    /// Como programador de aplicaciones no debes llamar a ninguna de estos metodos.
    class ITaskEventQueue
    {
        public:
            ITaskEventQueue() = default;

            virtual ~ITaskEventQueue() = default;

            /// Reenvia el siguiente evento al listaner del evento
            virtual void forward_to_event_listener() = 0;

            /// Retorna el tamanyo de la cola de eventos.
            virtual int size() = 0;

            virtual void register_notification(QueueNotification* notification) = 0;
    };
}
