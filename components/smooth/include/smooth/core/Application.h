#pragma once

#include <vector>
#include <chrono>
#include <unordered_map>
#include "smooth/core/ipc/Queue.h"
#include "smooth/core/Task.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/ipc/SubscribingTaskEventQueue.h"
#include "smooth/core/network/Wifi.h"

namespace smooth::core
{

    /// EarlyInit es utilizada para inicializar los recursor que necesitan ser inicializados antes 
    /// de la aplicacion.
    class EarlyInit : public Task
    {
        public:
            EarlyInit(uint32_t priority, const std::chrono::milliseconds& tick_interval);
            ~EarlyInit() override;
    };

    /// La aplicación se 'adjunta' a la tarea principal y le da al programador de la aplicación
    /// las mismas posibilidades para realizar el trabajo en la tarea principal como si se hubiera creado una tarea separada.
    /// Cualquier aplicación escrita basada en Smooth debe tener una instancia de la clase Aplicación
    /// (o una clase derivada de la Aplicación) en la pila en su app_main().
    /// Asegúrese de ajustar el tamaño de la pila de la tarea principal en consecuencia usando 'make menuconfig'.
    /// Nota: a diferencia de la versión de start() en Task, cuando se llama en una instancia de Aplicación, start() nunca regresa.    
    class Application
        : public EarlyInit
    {
        public:
            /// Constructor
            /// \param priority la prioridad con la que se ejecutara.
            /// Normalmente Usually smooth::core::APPLICATION_BASE_PRIO + un valor arbitrario,
            /// pero deberia ser menor que la prioridad de las tareas ESP-IDF como la del 
            /// driver de Wifi.
            /// \param tick_interval The tick interval
            Application(uint32_t priority, std::chrono::milliseconds tick_interval)
                    : EarlyInit(priority, tick_interval)
            {
            }

            void init() override;

            network::Wifi& get_wifi()
            {
                return wifi;
            }

        private:
            network::Wifi wifi{};
            
    };
}
