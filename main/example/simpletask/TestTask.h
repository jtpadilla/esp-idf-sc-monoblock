
#include <stdio.h>

#include "Task.h"
#include "GeneralUtils.h"

namespace example::simpletask
{

	class MyTask: public genielink::os::Task
	{

		public:

			MyTask(std::string taskName, uint32_t millisecsParam):
				genielink::os::Task{taskName}, millisecs {millisecsParam}
			{
			}

			void run(void* data) {

				int count = 0;

				while(count++ < 10) {
					printf("[%s] count: %d\n", m_taskName.c_str(), count);
					delay(millisecs);
				}

				genielink::sys::GeneralUtils::dumpInfo();
				printf("Done\n");

			}

		private:
			uint32_t millisecs;

	};

	extern void launch();

}