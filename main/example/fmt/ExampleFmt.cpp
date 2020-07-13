
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ExampleFmt.h"

#include "fmt/printf.h"

namespace example::fmt
{

	void launch() {

        while(true) {

            ::fmt::printf("Hello, %s!\n", "world");

            ::vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);

        }

	}

}