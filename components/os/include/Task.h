#define once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>

namespace genielink::os
{

	class Task {

		public:

			Task(std::string taskName = "Task", uint16_t stackSize = 10000, uint8_t priority = 5);
			virtual ~Task();

			void setStackSize(uint16_t stackSize);
			void setPriority(uint8_t priority);
			void setName(std::string name);
			void setCore(BaseType_t coreId);

			void start(void* taskData = nullptr);
			uint32_t getTimeSinceStart();
			void stop();

			virtual void run(void* data) = 0;

			static void delay(int ms);

		protected:
			std::string m_taskName;

		private:
			xTaskHandle m_handle;
			void*       m_taskData;
			uint16_t    m_stackSize;
			uint8_t     m_priority;
			BaseType_t  m_coreId;
			
			static void runTask(void* data);

	};

}