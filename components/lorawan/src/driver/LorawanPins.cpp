#include <string>
#include <cstdlib>

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"

#include "LorawanPins.h"

namespace genielink::lorawan::driver
{

    static const char *TAG = "genielink::lorawan::driver::lorawanpins";

    std::unique_ptr<LorawanPins> LorawanPins::instantiate(LorawanDevice lorawanDevice) {
        switch(lorawanDevice) {
            case LorawanDevice::TTGO_T_BEAM:
                //return std::unique_ptr<LorawanPins> {new LorawanPins{"TTGO_T_BEAM", HSPI_HOST, 1, 5, 27, 19, 18, NOT_CONNECTED, 23, 26, 33} };
                return std::make_unique<LorawanPins>("TTGO_T_BEAM", HSPI_HOST, 1, 5, 27, 19, 18, NOT_CONNECTED, 23, 26, 33);
            default:
                abort();
        }
    }

    LorawanPins::LorawanPins(
        std::string name_arg,
        spi_host_device_t spi_host_arg,
        uint8_t spi_dma_chan_arg,
        uint8_t pin_spi_sclk_arg,
        uint8_t pin_spi_mosi_arg,
        uint8_t pin_spi_miso_arg,
        uint8_t pin_nss_arg,
        uint8_t pin_rxtx_arg,
        uint8_t pin_rst_arg,
        uint8_t pin_di00_arg,
        uint8_t pin_di01_arg):
        name {name_arg},
        spi_host {spi_host_arg},
        spi_dma_chan {spi_dma_chan_arg},
        pin_spi_sclk {pin_spi_sclk_arg},
        pin_spi_mosi {pin_spi_mosi_arg},
        pin_spi_miso {pin_spi_miso_arg},
        pin_nss {pin_nss_arg},
        pin_rxtx {pin_rxtx_arg},
        pin_rst {pin_rst_arg},
        pin_di00 {pin_di00_arg},
        pin_di01 {pin_di01_arg}
    {

        esp_err_t err;
        
        // Initialize the GPIO ISR handler service
        ESP_LOGD(TAG, "Se instala el GPIO Isr service");
        err = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
        ESP_ERROR_CHECK(err);

        // Initialize SPI bus
        ESP_LOGD(TAG, "Se inicializa el bus SPI");
        spi_bus_config_t spi_bus_config{};
        spi_bus_config.miso_io_num = pin_spi_miso;
        spi_bus_config.mosi_io_num = pin_spi_mosi;
        spi_bus_config.sclk_io_num = pin_spi_sclk;
        spi_bus_config.quadwp_io_num = -1;
        spi_bus_config.quadhd_io_num = -1;
        spi_bus_config.max_transfer_sz = 0;
        err = spi_bus_initialize(spi_host, &spi_bus_config, spi_dma_chan);
        ESP_ERROR_CHECK(err);

    }

    std::string LorawanPins::get_name() const {
        return name;
    }

    spi_host_device_t LorawanPins::get_spi_host() const {
        return spi_host;
    }

    uint8_t LorawanPins::get_spi_dma_chan() const {
        return spi_dma_chan;
    }

    uint8_t LorawanPins::get_pin_spi_sclk() const {
        return pin_spi_sclk;
    }

    uint8_t LorawanPins::get_pin_spi_mosi() const {
        return pin_spi_mosi;
    }

    uint8_t LorawanPins::get_pin_spi_miso() const {
        return pin_spi_miso;
    }

    uint8_t LorawanPins::get_pin_nss() const {
        return pin_nss;
    }

    uint8_t LorawanPins::get_pin_rxtx() const {
        return pin_rxtx;
    }

    uint8_t LorawanPins::get_pin_rst() const {
        return pin_rst;
    }

    uint8_t LorawanPins::get_pin_di00() const {
        return pin_di00;
    }

    uint8_t LorawanPins::get_pin_di01() const {
        return pin_di01;
    }

}