#pragma once

namespace smooth::core::ipc
{

    /// Implementa este interface por cada tipo de evento que desees recibir desde un TaskEventQueue
    /// \tparam T
    template<typename EventType>
    class IEventListener
    {
        public:
            virtual ~IEventListener() = default;

            /// El metodo de respuesta donde el evento sera recibido desde un TaskEventQueue<EventType>
            /// \param event The event
            virtual void event(const EventType& event) = 0;
    };
    
}
