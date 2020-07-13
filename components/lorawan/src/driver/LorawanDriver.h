
#pragma once

#include <stdint.h>
#include <memory.h>

#include "driver/spi_master.h"

#include "LorawanDevice.h"
#include "LorawanClient.h"
#include "LorawanPins.h"
#include "LorawanParameters.h"
#include "LorawanListener.h"
#include "LorawanHAL.h"

namespace genielink::lorawan::driver
{

    /**
     * @brief Lorawan device
     * 
     * The 'Lorawan' class enables ESP32 devices with SX1272/73/76/77/78/79 LoRaWAN chips
     * to communicate via The Things Network.
     * 
     * Only one instance of this class must be created.
     */
    class LorawanDriver
    {

        public:

            /**
             * @brief Crea la instancia del driver
             * 
             * @param lorawanPinsArg configuracion de pins que utilizara el driver
             * @param lorawanParametersArg Parametros utilizados para la conexion con la red
             * 
             * Solo puede ser creada una instancia
             */
            static std::unique_ptr<LorawanDriver> instantiate(const LorawanDevice lorawanDevice, const LorawanParameters& lorawanParameters);

            /**
             * @brief Construct a new The Things Network device instance.
             */
            LorawanDriver();

            /**
             * @brief Destroy the The Things Network device instance.
             */
            ~LorawanDriver();

            void installListener(ILorawanListener *lorawanListener);

            /**
             * @brief Reset the LorawanDriver radio.
             * 
             * Does not clear provisioned keys.
             */
            void reset();

            /**
             * @brief Activate the device via OTAA.
             * 
             * The app EUI, app key and dev EUI must already have been provisioned by a call to 'provision()'.
             * Before this function is called, 'nvs_flash_init' must have been called once.
             * 
             * The function blocks until the activation has completed or failed.
             * 
             * @return true   if the activation was succeful
             * @return false  if the activation failed
             */
            bool join();

            /**
             * @brief Transmit a message
             * 
             * The function blocks until the message could be transmitted and a message has been received
             * in the subsequent receive window (or the window expires). Additionally, the function will
             * first wait until the duty cycle allows a transmission (enforcing the duty cycle limits).
             * 
             * @param payload  bytes to be transmitted
             * @param length   number of bytes to be transmitted
             * @param port     port (default to 1)
             * @param confirm  flag indicating if a confirmation should be requested. Default to 'false'
             * @return TSuccessfulTransmission   Successful transmission
             * @return ErrorTransmissionFailed   Transmission failed
             * @return TErrorUnexpected          Unexpected error
             */
            genielink::lorawan::LorawanResponse transmitMessage(const uint8_t *payload, size_t length, port_t port, bool confirm);

            /**
             * @brief Sets the RSSI calibration value for LBT (Listen Before Talk).
             * 
             * This value is added to RSSI measured prior to decision. It must include the guardband.
             * Ignored in US, EU, IN and other countries where LBT is not required.
             * Default to 10 dB.
             * 
             * @param rssiCal RSSI calibration value, in dB
             */
            void setRSSICal(int8_t rssiCal);

        private:
            ILorawanListener *listener;

    };

}

