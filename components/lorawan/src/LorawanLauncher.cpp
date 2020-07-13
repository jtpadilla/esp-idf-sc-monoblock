#include <memory>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_event.h"

#include "LorawanLauncher.h"
#include "LorawanListener.h"
#include "LorawanActivator.h"
#include "LorawanClientImpl.h"

static const char* TAG = "genielink::lorawan::LorawanLauncher::launch";

namespace genielink::lorawan
{

    void LorawanLauncher::launch(const LorawanDevice lorawanDevice, const LorawanParameters& lorawanParameters, ILorawanActivator& lorawanActivator, size_t retrySeconds) {

        // Se instancia el cliente
        std::unique_ptr<LorawanClientImpl> lorawanClientImpl = std::make_unique<LorawanClientImpl>(LorawanClientImpl(lorawanDevice, lorawanParameters));

        // Intentos indefinidos de hacer el join
        ESP_LOGI(TAG, "Joining...");
        while (!lorawanClientImpl->join()) {
            ESP_LOGI(TAG, "Join retry...");
            vTaskDelay(retrySeconds * 1000 / portTICK_PERIOD_MS);
        }

        // Ya estamos en la red y se puede activar el cliente Lorawan
        ESP_LOGI(TAG, "Joined!");

        // se le encia al activador el puntero al cliente y se convierte en el propietario
        lorawanActivator.lorawanActivate(std::move(lorawanClientImpl));

    }

}