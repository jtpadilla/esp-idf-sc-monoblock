#pragma once

#include <cstdint>

namespace smooth::core
{
    // Estas son las prioridades utilizadas por las diferentes tareas
    // del framework.
    // Manten la prioridades de las tareas de tu aplicacion mas bajas
    // que las prioridades de los servicios del framework para asegurar
    // el funcionamiento del sistema.
    const uint32_t APPLICATION_BASE_PRIO = 5;
    const uint32_t TIMER_SERVICE_PRIO = 19;
    const uint32_t SOCKET_DISPATCHER_PRIO = 20;
}
