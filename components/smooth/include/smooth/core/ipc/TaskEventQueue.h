#pragma once

#include <memory>

#include "smooth/core/Task.h"
#include "ITaskEventQueue.h"
#include "IEventListener.h"
#include "QueueNotification.h"
#include "smooth/core/util/create_protected.h"

namespace smooth::core::ipc
{

    /// TaskEventQueue expande la funcionalidad de Queue<T>, junto con la Task, agregando la capacidad
    /// para señalar una Task cuando un elemento está disponible, haciendo innecesario el sondeo 
    /// de una Queue, lo que libera la tarea para hacer otras cosas.
    /// \tparam T El tipo de eventos que se recibira.
    template<typename T>
    class TaskEventQueue : public ITaskEventQueue, public std::enable_shared_from_this<TaskEventQueue<T>>
    {
        public:

            friend core::Task;

            static_assert(std::is_default_constructible<T>::value, "DataType must be default-constructible");
            static_assert(std::is_assignable<T, T>::value, "DataType must be a assignable");

            static auto create(int size, Task& owner_task, IEventListener<T>& event_listener)
            {
                return smooth::core::util::create_protected_shared<TaskEventQueue<T>>(size, owner_task, event_listener);
            }

            ~TaskEventQueue() override
            {
                notif->remove_expired_queues();
            }

            // Se suprimen el constructor por defecto, contructor copia, operador de asinacion
            // costructor move y copia move.
            TaskEventQueue() = delete;
            TaskEventQueue(const TaskEventQueue&) = delete;
            TaskEventQueue(TaskEventQueue&&) = delete;
            TaskEventQueue& operator=(const TaskEventQueue&) = delete;
            TaskEventQueue& operator=(const TaskEventQueue&&) = delete;

            /// Empuja un item en la cola
            /// \param item El item del cual una copia sera puesta en la cola.
            /// \return true si la cola a aceptado el item, false en caso contrario.
            virtual bool push(const T& item)
            {
                return push_internal(item, this->shared_from_this());
            }

            /// Obtiene el numero de items que la cola puede almacenar.
            /// \return numero de items que la cola puede almacenar.
            int size() override
            {
                return queue.size();
            }

            // Retorna el numero de items que estan esperando a ser recuperados
            /// \return El numero de itmes en la cola
            int count()
            {
                return queue.count();
            }

            void register_notification(QueueNotification* notification) override
            {
                notif = notification;
            }

            void clear()
            {
                while (!queue.empty())
                {
                    T t;
                    queue.pop(t);
                }
            }

        protected:

            /// Constructor
            /// \param size Numero de items que puede almacenar la cola.
            /// \param task Tarea a la que se le senyalara cuando un evento esta disponible
            /// \param listener El receptor de los eventos. Normalmente es el mismo que la 
            /// tarea pero puede ser cualquier instcna de un objeto.
            TaskEventQueue(int size, Task& task, IEventListener<T>& listener)
                : queue(size), task(task), listener(listener)
            {
                task.register_queue_with_task(this);
            }

            bool push_internal(const T& item, const std::weak_ptr<ITaskEventQueue>& receiver)
            {
                auto res = queue.push(item);

                if (res)
                {
                    notif->notify(receiver);
                }

                return res;
            }

            template<typename Derived>
            std::shared_ptr<Derived> shared_from_base()
            {
                return std::static_pointer_cast<Derived>(this->shared_from_this());
            }

            Queue<T> queue;
            QueueNotification* notif = nullptr;

        private:

            void forward_to_event_listener() override
            {

                // Todos los mensajes pasados a una cola necesitan un contructor por defecto
                // y tiene que ser copiables y tener un operador de asignacion.
                T m;

                if (queue.pop(m))
                {
                    listener.event(m);
                }
            }

            Task& task;
            IEventListener<T>& listener;

    };

}
