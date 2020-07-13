#pragma once

#include <string>
#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

#include "smooth/core/ipc/ITaskEventQueue.h"
#include "smooth/core/ipc/IPolledTaskQueue.h"
#include "smooth/core/ipc/QueueNotification.h"
#include "smooth/core/ipc/Queue.h"
#include "smooth/core/timer/ElapsedTime.h"

#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#pragma GCC diagnostic pop
#include <sdkconfig.h>

#if CONFIG_FREERTOS_HZ < 1000
#error "Smooth requires CONFIG_FREERTOS_HZ at 1000Hz for proper functionality (spec. timers)"
#endif

namespace smooth::core
{
    /// La clase Task encapsula la gestión y ejecución de una tarea.
    /// La intención es proporcionar el andamiaje necesario para casi todas las tareas en un
    /// sistema embebido; un método de inicialización, uno llamado periódicamente tick(),
    /// la capacidad de recibir eventos de manera segura para subprocesos.
    class Task
    {
        
        public:
            virtual ~Task();

            /// Empieza la tarea
            void start();
            void register_queue_with_task(smooth::core::ipc::ITaskEventQueue* task_queue);
            void register_polled_queue_with_task(smooth::core::ipc::IPolledTaskQueue* polled_queue);
            void unregister_polled_queue_with_task(smooth::core::ipc::IPolledTaskQueue* polled_queue);

            Task(const Task&) = delete;
            Task& operator=(const Task&) = delete;
            Task(Task&&) = delete;
            Task& operator=(Task&&) = delete;

        protected:
        
            // Usa este constructor para unirse a una terea existente (ej: la tarea principal)
            /// \param priority Prioridad de la tarea
            /// \param tick_interval Intervaloe del Tick
            Task(uint32_t priority, std::chrono::milliseconds tick_interval);

            /// Usa este metodo para crear tu propia tarea.
            /// \param task_name Nombre de la tarea.
            /// \param stack_size Dimension del stack en bytes.
            /// \param priority Prioridad de la tarea
            /// \param tick_interval Intervalo del tick
            /// \param core Afinidad del core del procesados, por defecto sin afinidad.
            Task(std::string task_name,
                 uint32_t stack_size,
                 uint32_t priority,
                 std::chrono::milliseconds tick_interval,
                 int core = tskNO_AFFINITY);

            /// El metodo tick() es donde la tarea deberia de realizar su trabajo.
            /// Este metodo es llamado cada 'tick_interval' cuando no hay eventos disponibles.
            /// Hay que tener en cuenta que si un flujo constante de eventos es recibido a traves
            /// de TaskEventQueue, el tick sera retrasado (dependeiendo del tick_interval).
            virtual void tick()
            {
            }

            /// Llamado una vez cuando se inicia la tarea.
            virtual void init()
            {
            }

            void report_stack_status();

            const std::string name;

        private:

            void exec();

            // Thread subyacente de la tarea
            std::thread worker;

            // Dimension del stack en bytes
            uint32_t stack_size;

            // Prioridad de la nueva tarea
            uint32_t priority;

            // El constructor informara este valor indicando si hay un nuevo thread o no
            bool is_attached;

            // Core que se utilizara
            int affinity;

            // Cada cuanto tiempo se dispara el tick
            std::chrono::milliseconds tick_interval;

            // Indica si la tarea ha sido iniciada
            std::atomic_bool started{ false };

            // Mutex que permite sincronizar el arranque de las distintas tareas.
            std::mutex start_mutex{};

            // El thread de la tarea puede comunicar al threda lanzador que ya esta iniciado.
            std::condition_variable start_condition{};

            // Permite determinar cuando hay que actualizar las estadisticas de la tarea
            smooth::core::timer::ElapsedTime status_report_timer{};

            smooth::core::ipc::QueueNotification notification{};
            std::mutex queue_mutex{};
            std::vector<smooth::core::ipc::IPolledTaskQueue*> polled_queues{};

    };
    
}
