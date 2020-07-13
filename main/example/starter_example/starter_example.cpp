
#include "starter_example.h"
#include "smooth/core/Task.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/SystemStatistics.h"

using namespace smooth::core;

namespace starter_example
{
    class ATask : public smooth::core::Task
    {

        public:
            ATask() 
                : smooth::core::Task("Task", 9000, APPLICATION_BASE_PRIO, std::chrono::seconds{ 1 })
            {
            }

            void init() override
            {
                Log::info("Task::init", "Iniciando la tarea..");
            }

            void tick() override
            {
                Log::info("Task::tick", "Hola desde la tarea!");
            }

    };

    ATask a_instance{};

    App::App()
        : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(3))
    {
    }

    void App::init()
    {
        Log::info("App::Init", "Iniciando la apliocacion..");
        a_instance.start();
    }

    void App::tick()
    {
        Log::info("App::Tick", "Hola desde la aplicacion!");
        SystemStatistics::instance().dump();
    }

}
