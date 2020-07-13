#define once

#include <string>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace genielink::os
{
    
    class Semaphore {

		public:

			Semaphore();
			~Semaphore();

			void give();
			void giveFromISR();
			bool take();
			bool take(uint32_t timeoutMs);
			void wait();

		private:
			SemaphoreHandle_t semaphore;

    };

}