#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <mutex>
#include <algorithm>
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::core::ipc
{

    /// T Queue<T> es precisamente lo que el nombre sugiere - una cola que almacena items de tipo T.
    /// Es segura para utilizarse desde distintos threads. Se puede utilizar como una 
    /// cola independiante o como base para una cola mas especializada como TaskEventQueue y
    /// SubscribingTaskEventQueue.
    /// Esta cola soporta objetos reales C++ a diferencia de los datos planos de FreeRTOS.
    /// Esto significa que puedes colocar en la cola cualquier tipo de objeto C++ simpre y cuando 
    /// sean copiables (el contructor de copia y el operador de asignacion seran suficientes)
    /// Los items son colocados en la cola por copia, no por referencia.
    /// \tparam T El tipo de objeto a colocar en la cola.
    template<typename T>
    class Queue
    {
        public:
            /// Constructor
            /// \param name Nombre de la cola, utilizada para debug y log.
            /// \param size Dimension de la cola, numero de items que puede almacenar.
            explicit Queue(int size)
                    : queue_size(size),
                      items(),
                      guard()
            {
                items.reserve(static_cast<size_t>(size));
            }

            /// Destructor
            virtual ~Queue()
            {
                std::lock_guard<std::mutex> lock(guard);
                items.clear();
            }

            /// \return Obtiene la dimension de la cola
            int size()
            {
                std::lock_guard<std::mutex> lock(guard);

                return queue_size;
            }

            /// Empuja un item dentro de la cola
            /// \param item El item del cual un item sera puesto en la cola.
            /// \return true si la cola a podido aceptar el item, false en caso contrario.
            bool push(const T& item)
            {
                std::lock_guard<std::mutex> lock(guard);

                bool res = items.size() < static_cast<size_t>(queue_size);

                if (res)
                {
                    items.emplace_back(item);
                }

                return res;
            }

            /// Saca un elemento de la cola
            /// \param target Una referencia a la instancia de T al que le sera asignado el item sacado de la cola..
            /// \return true si el item a podido ser recibidi, false en caso contrario
            bool pop(T& target)
            {
                std::lock_guard<std::mutex> lock(guard);

                bool res = items.size() > 0;

                if (res)
                {
                    target = items.front();
                    items.erase(items.begin());
                }

                return res;
            }

            /// Retorna un valor indicando si la cola esta vacia.
            /// \return true si esta vacia, false en caso contrario.
            bool empty()
            {
                return count() == 0;
            }

            /// Retorna el numero de items que estan esperando a ser sacados.
            /// \return Numero de items de la cola.
            int count()
            {
                std::lock_guard<std::mutex> lock(guard);

                return static_cast<int>(items.size());
            }

        private:
            const int queue_size;
            std::vector<T> items;
            std::mutex guard;
            
    };
}
