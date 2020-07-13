#include <utility>
#include <algorithm>
#include "smooth/core/Task.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/ipc/Publisher.h"
#include "smooth/core/SystemStatistics.h"

#include <esp_pthread.h>
#include <freertos/task.h>

using namespace smooth::core::logging;

namespace smooth::core
{
    /// Constructor utilizado cuando se crea una nueva tarea ejecutandose
    /// en un nuevo thread.
    Task::Task(std::string task_name, uint32_t stack_size, uint32_t priority,
               std::chrono::milliseconds tick_interval, int core)
        :   name(std::move(task_name)),
            worker(),
            stack_size(stack_size),
            priority(priority),
            is_attached(false),
            affinity(core),
            tick_interval(tick_interval)
    {
    }

    /// Constructor utilizado cuando la nueva tarea se engancha al thread 
    /// que ejecuta el constructor.
    Task::Task(uint32_t priority, std::chrono::milliseconds tick_interval)
        :   name("MainTask"),
            stack_size(0),
            priority(priority),
            is_attached(true),
            affinity(tskNO_AFFINITY),
            tick_interval(tick_interval)
    {
        stack_size = CONFIG_MAIN_TASK_STACK_SIZE;
    }

    Task::~Task()
    {
        notification.clear();
    }

    void Task::start()
    {

        // Obtiene el acceso esclusivo para poder iniciar el arranque de esta tarea
        std::unique_lock<std::mutex> lock{ start_mutex };

        // Evita multiples starts..
        if (!started)
        {

            // Se inicia el timer para las actualizaciones de las estadisticas de la tarea
            status_report_timer.start();

            if (is_attached)
            {
                Log::debug(name, "Running as attached thread");

                // En lugar de crear un nuevo thread, se ejecuta con el threda
                // que a invocado este metodo.
                exec();
            }
            else
            {

                // Se utilizara std::thread que esta implementado con pthread.
                // Hay que gestionar la configuracion del sistema que es la utilizada 
                // en el momento de crear el thread.

                // Se obtiene la configuracion para los pthread que tiene el sistema
                auto worker_config = ::esp_pthread_get_default_config();

                // Se ajusta la configuracion a la requerida para el nuevo thread
                worker_config.stack_size = stack_size;
                worker_config.prio = priority;
                worker_config.thread_name = name.c_str();

                // Se se ha especificado un core se configura.
                if (affinity != tskNO_AFFINITY)
                {
                    worker_config.pin_to_core = affinity;
                }

                // Se establece la nueva configuracion del sistema para los threads
                ::esp_pthread_set_cfg(&worker_config);

                // Se inicia el nuevo thread para ejecutar la tarea
                Log::debug(name, "Creating worker thread");
                worker = std::thread([this]() {this->exec();});

                // Para evitar 'condiciones de carrera' entre las tareas 
                // durante su inicio, siempre se espera que la tarea arranque.
                Log::debug(name, "Waiting for worker to start");
                start_condition.wait(lock, [this] { return started.load(); });
                Log::debug(name, "Worker started");
                
            }
        }
    }

    void Task::exec()
    {
        Log::debug(name, "Executing...");

        // Si se ha creado un nuevo thread, se le indica al llamante que ya se ha iniciado.
        if (!is_attached)
        {
            Log::debug(name, "Notify start_mutex");
            std::unique_lock<std::mutex> lock{ start_mutex };
            started = true;
            start_condition.notify_all();
        }

        // Se llama al codigo de inicializacion del usuario
        Log::verbose(name, "Initializing...");
        init();
        Log::verbose(name, "Initialized");

        // ElapsedTime que nos permite detectar el momnto de hacer un 'tick()'
        timer::ElapsedTime delayed{};
        delayed.start();

        // Antes de empezar actualizamos las estadisticas de la tarea
        report_stack_status();

        // Bucle infinito de la tarea
        for (;; )
        {

            // El tick tiene preferencia se le toca se ejecuta aunque hayan muchos mensajes  entrantes.
            if (tick_interval.count() > 0 && delayed.get_running_time() > tick_interval)
            {
                // Ya toca tick!
                // Da igual que hayan eventos pendientes.
                tick();
                delayed.reset();

            }
            else
            {

                // No toca tick!
                // Podemos dedicarnos a lso eventos..

                // Obtiene el acceso exclusivo a la cola
                std::unique_lock<std::mutex> lock{ queue_mutex };

                // Check the polled queues for data availability
                for (auto q : polled_queues)
                {
                    q->poll();
                }

                // Wait for data to become available, or a timeout to occur.
                auto queue_ptr = notification.wait_for_notification(tick_interval);

                // Check if the weak_ptr we got back is uninitialized using the defined behaviour here:
                // https://en.cppreference.com/w/cpp/memory/weak_ptr/owner_before
                //
                // Quote: The order is such that two smart pointers compare equivalent only
                // if they are both empty or if they both own the same object, even if the
                // values of the pointers obtained by get() are different (e.g. because they
                // point at different subobjects within the same object)
                decltype(queue_ptr) empty_ptr{};

                if (!queue_ptr.owner_before(empty_ptr) && !empty_ptr.owner_before(queue_ptr))
                {
                    // Timeout - no messages.
                    tick();
                    delayed.reset();
                }
                else
                {
                    // A queue has signaled an item is available.
                    // Note: Do not retrieve all messages from the the queue;
                    // it will prevent messages to arrive in the same order
                    // they were sent when there are more than one receiver queue.
                    auto queue = queue_ptr.lock();

                    if (queue)
                    {
                        queue->forward_to_event_listener();
                    }
                }
                
            }

            // si han pasado 60 segundos desde la ultima actualizacion se actualiza de nuevo.
            if (status_report_timer.get_running_time() > std::chrono::seconds(60))
            {
                report_stack_status();
                status_report_timer.reset();
            }

        }

    }

    // Cuando de crea una instancia de TaskEventQueue, esta se crea asociada a una Task.
    // El constructos de TaskEventQueue recibe como parametro la Task con la que se asocia,
    // pero la referencia a la Task no se almacena, simplemente el constructor de TaskEventQueue
    // invoca este metodo pasando su propia referencia (this) para que la Task finalice
    // la configuracion de TaskEventQueue enlazandolo con el correspondiente QueueNotification
    // que corresponde a esta Task.
    void Task::register_queue_with_task(smooth::core::ipc::ITaskEventQueue* task_queue)
    {
        task_queue->register_notification(&notification);
    }

    void Task::register_polled_queue_with_task(smooth::core::ipc::IPolledTaskQueue* polled_queue)
    {
        std::unique_lock<std::mutex> lock{ queue_mutex };
        polled_queue->register_notification(&notification);
        polled_queues.push_back(polled_queue);
    }

    void Task::unregister_polled_queue_with_task(smooth::core::ipc::IPolledTaskQueue* polled_queue)
    {
        std::unique_lock<std::mutex> lock{ queue_mutex };
        auto pos = std::find(polled_queues.begin(), polled_queues.end(), polled_queue);

        if (pos != polled_queues.end())
        {
            polled_queues.erase(pos);
        }
    }

    void Task::report_stack_status()
    {
        SystemStatistics::instance().report(name, TaskStats{ stack_size });
    }
    
}
