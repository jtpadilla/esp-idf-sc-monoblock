#pragma once

namespace smooth::core::ipc
{
    template<typename T>
    class ILinkSubscriber
    {
        public:
            virtual ~ILinkSubscriber() = default;

            virtual bool receive_published_data(const T& data) = 0;
    };
}
