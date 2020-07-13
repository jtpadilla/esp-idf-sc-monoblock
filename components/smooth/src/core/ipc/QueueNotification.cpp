#include <thread>
#include "smooth/core/ipc/QueueNotification.h"
#include <algorithm>

namespace smooth::core::ipc
{

    // Puede parecer que la cola puede crecer sin límites, pero ese no es el caso
    // como TaskEventQueues solo llama a este método cuando han agregado correctamente
    // elemento de datos a su cola interna. Como tal, la cola solo puede ser tan grande como
    // la suma de todas las colas dentro de la misma Tarea.
    void QueueNotification::notify(const std::weak_ptr<ITaskEventQueue>& queue)
    {
        // Se obtiene el acceso esclusivo
        std::unique_lock<std::mutex> lock{ guard };

        // Se coloca el nuevo ITaskEventQueue al final de la cola
        queues.emplace_back(queue);

        // Se notifica que hay mas entradas en la cola
        cond.notify_one();

    }

    void QueueNotification::remove_expired_queues()
    {
        std::unique_lock<std::mutex> lock{ guard };

        auto new_end = std::remove_if(queues.begin(), queues.end(), [&](const auto& o) { return o.expired(); });

        queues.erase(new_end, queues.end());
    }

    std::weak_ptr<ITaskEventQueue> QueueNotification::wait_for_notification(std::chrono::milliseconds timeout)
    {

        // Se obtien el acceso exclusivo
        std::unique_lock<std::mutex> lock{ guard };

        // Se prepara donde recoger el resultado 
        std::weak_ptr<ITaskEventQueue> res{};

        // Hay un comportamiendo distinto dependiendo de si la lista tiene o no entradas
        if (queues.empty())
        {

            // Hay que esperar hasta que:
            //   1) Hayan datos disponibles
            //   2) Haya transcurrido el tiempo de timeout
            // ESTUDIAR -> Wait until data is available, or timeout. This will atomically release the lock.
            auto wait_result = cond.wait_until(lock,
                                               std::chrono::steady_clock::now() + timeout,
                                               [this]() {
                                                   // Stop waiting when there is data
                                                   return !queues.empty();
                                               });

            // ESTUDIAR -> At this point we will have the lock again.
            if (wait_result)
            {
                // La cola ahora ya tiene entradas
                if (!queues.empty())
                {
                    // Hay entredas, asi que se puede extraer y retornar el resultado
                    res = queues.front();
                    queues.pop_front();
                }
            }
        }
        else
        {
            // Hay entredas, asi que se puede extraer y retornar el resultado
            res = queues.front();
            queues.pop_front();
        }

        // 'res' puede no tener cargada ninguna referencia 
        // (si se ha cuberto el timeout esperando nuevas entradas)
        return res;
        
    }
    
}
