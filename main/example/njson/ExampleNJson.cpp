#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ExampleNJson.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include "fmt/printf.h"

namespace example::njson
{

	void launch() {

        while(true) {

            // create a JSON array
            json j1 = {"one", "two", 3, 4.5, false};

            // create a copy
            json j2(j1);

            // serialize the JSON array
            std::cout << j1 << " = " << j2 << '\n';
            std::cout << std::boolalpha << (j1 == j2) << '\n';

            ::vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);

        }

	}

}