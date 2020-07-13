
#include <string>

#include "esp_log.h"

#include "LorawanParameters.h"
#include "LorawanUtil.h"
#include "LorawanClient.h"
#include "LorawanLauncher.h"
#include "LorawanDevice.h"
#include "SCExampleLorawanTask.h"
#include "SCExampleLauncher.h"

namespace example::lorawan
{

    constexpr char devEui[] = "004CFEED74AD2FA6";
    constexpr char appEui[] = "70B3D57ED00306F7";
    constexpr char appKey[] = "8214F6A2800C9FCD9B26BBE28D5CD057";

    static const char* TAG = "example::lorawan::launch";

    void launch(void)
    {

        ESP_LOGI(TAG, "Se lanza la tarea principal");
        ExampleLorawanTask exampleLorawanTask {};
        exampleLorawanTask.start();

        try
        {

            ESP_LOGI(TAG, "Se convierten los parametros Lorawan en texto en el formato binario");
            genielink::lorawan::LorawanParameters lorawanParameters = genielink::lorawan::LorawanUtil::convert(std::string{appEui}, std::string{appKey}, std::string{devEui});

            ESP_LOGI(TAG, "Se lanza la tarea del Join.");
            JoinTask joinTask {LorawanDevice::TTGO_T_BEAM, lorawanParameters, exampleLorawanTask};
            joinTask.start();

            ESP_LOGI(TAG, "En thread principal se queda esperando para siempre.");
            while (1) {
                vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
            }


        }
        catch (genielink::lorawan::LorawanException &lorawanParamsEx)
        {
            ESP_LOGE(TAG, "Los parametros Lorawan en texto no se han podido convertir en el formato binario.");
        }

    }

}