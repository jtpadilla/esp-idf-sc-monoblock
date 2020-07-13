#include "example/task_event_queue/task_event_queue.h"

using namespace task_event_queue;

extern "C" void app_main()
{
    App app{};
    app.start();
}
