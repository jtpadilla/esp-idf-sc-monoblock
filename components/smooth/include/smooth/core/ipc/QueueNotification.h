#pragma once

#include <condition_variable>
#include <mutex>
#include <deque>
#include <memory>
#include "ITaskEventQueue.h"

namespace smooth::core::ipc
{
    class QueueNotification
    {
        public:

            QueueNotification() = default;
            ~QueueNotification() = default;

            void notify(const std::weak_ptr<ITaskEventQueue>& queue);
            void remove_expired_queues();
            std::weak_ptr<ITaskEventQueue> wait_for_notification(std::chrono::milliseconds timeout);

            void clear()
            {
                std::lock_guard<std::mutex> lock(guard);
                queues.clear();
            }

        private:
            std::deque<std::weak_ptr<ITaskEventQueue>> queues{};
            std::mutex guard{};
            std::condition_variable cond{};
    };
}
