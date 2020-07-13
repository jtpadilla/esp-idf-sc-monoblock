
#include "task_event_queue.h"

#include "smooth/core/logging/log.h"
#include "smooth/core/task_priorities.h"

#include <utility>

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace task_event_queue
{
    App::App()
        :   Application(APPLICATION_BASE_PRIO, std::chrono::milliseconds(1000)),
            queue(ElapsedTimeQueue::create(10, *this, *this)),
            sender(queue)
    {
    }

    void App::init()
    {
        // Se invoca ::init de la clase padre
        Application::init();

        // Se inicial la tarea SenderTask
        sender.start();

    }

    void App::tick()
    {
        Log::info("App::tick", "Hola!");
    }

    void App::event(const timer::ElapsedTime& event)
    {

        auto val = event.get_running_time();
        total += val;
        count++;

        if (count % 100 == 0)
        {
            Log::info("Stats", "Avg: {}us per message, Count: {}, Last msg: {}us",
                                       static_cast<double>(total.count()) / count,
                                       // 2,
                                       count,
                                       val.count());
        }
    }

    SenderTask::SenderTask(std::weak_ptr<TaskEventQueue<timer::ElapsedTime>> out)
        :   core::Task("SenderTask", 4096, 10, milliseconds(1)),
            out(std::move(out))
    {
    }

    void SenderTask::tick()
    {

        timer::ElapsedTime e;
        e.start();
        auto q = out.lock();

        if (q)
        {
            q->push(e);
        }
    }

}
