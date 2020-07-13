#pragma once

#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/timer/ElapsedTime.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/Application.h"

namespace task_event_queue
{
    using ElapsedTimeQueue = smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime>;

    class SenderTask : public smooth::core::Task
    {
        public:
            explicit SenderTask(std::weak_ptr<ElapsedTimeQueue> out);

            void tick() override;

        private:
            std::weak_ptr<smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime>> out;
    };

    class App : public smooth::core::Application, public smooth::core::ipc::IEventListener<smooth::core::timer::ElapsedTime>
    {
        public:
            App();

            void init() override;
            void tick() override;

            uint32_t count = 0;
            std::chrono::microseconds total = std::chrono::microseconds(0);

            void event(const smooth::core::timer::ElapsedTime& event) override;

        private:
            std::shared_ptr<ElapsedTimeQueue> queue;
            SenderTask sender;
    };
}
