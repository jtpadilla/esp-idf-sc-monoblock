#include <string>
#include <cstdlib>
#include <exception>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "SCExampleLorawanTask.h"

namespace example::lorawan
{

    static const char* TAG = "ExampleLorawanTask";

    ExampleLorawanTask::ExampleLorawanTask(): 
        genielink::os::Task{"exampleloratask", 20000, 5},
        lorawanClientAccess{}, lorawanClient{nullptr}, 
        deviceId {0}, 
        configured {false},
        counterStatistics {}
    {
    }

    // implements -> genielink::lorawan::IlorawanActivator
    void ExampleLorawanTask::lorawanActivate(std::unique_ptr<genielink::lorawan::ILorawanClient> lorawanClientArg) {

        // Se obtiene el acceso esclusivo para asignar el cliente Lorawan
        if (lorawanClientAccess.take()) 
        {
            lorawanClient = std::move(lorawanClientArg);

            // Hay que liberar el acceso esclusivo
            lorawanClientAccess.give();
        } 
        else
        {
            ESP_LOGE(TAG, "No se ha podido obtener al ecceso para asignar el cliente Lorawan");
        } 

        //ESP_LOGI(TAG, "Se instala el listener para recibir los mensajes bajantes");
        lorawanClient->installListener(this);
    }

    // implements -> genielink::lorawan::ILorawanListener
    void ExampleLorawanTask::lorawanMessageReceived(const uint8_t* payload, size_t length, port_t port) {

        switch (port)
        {
        case 20:
            // Se recibe el identificador que se le ha asignado a este dispositivo
            {
                const char *msgPointer = reinterpret_cast<const char *>(payload);
                std::string msgString {msgPointer, length};
                try 
                {
                    if (!configureDevice(std::stoi(msgString)))
                    {
                        ESP_LOGE(TAG, "No se ha podido configurar el dispositivo!");
                    }
                }
                catch (std::exception &ex)
                {
                    ESP_LOGE(TAG, "exception: El recibir la configuracion del dispositivo!");
                }
            }
            break;
        
        case 21:
            // Se recibe un nuevo valor del contador confirmado.
            {
                const char *msgPointer = reinterpret_cast<const char *>(payload);
                std::string msgString {msgPointer, length};
                try 
                {
                    if (!receivedConfirmedCounter(std::stoi(msgString)))
                    {
                        ESP_LOGE(TAG, "No se ha podido actualizar la estadistica del contador confirmado!");
                    }
                }
                catch (std::exception &ex)
                {
                    ESP_LOGE(TAG, "exception: El recibir el contador confirmado!");
                }
            }
            break;
        
        case 22:
            // Se recibe un nuevo valor del contador global.
            {
                const char *msgPointer = reinterpret_cast<const char *>(payload);
                std::string msgString {msgPointer, length};
                try 
                {
                    if (!receivedUnconfirmedCounter(std::stoi(msgString)))
                    {
                        ESP_LOGE(TAG, "No se ha podido actualizar las estadisticas del contador no confirmado!");
                    }
                }
                catch (std::exception &ex)
                {
                    ESP_LOGE(TAG, "exception: El recibir el contador noconfirmado!");
                }
            }
            break;
        
        default:
            //ESP_LOGE(TAG, "DOWN-ERROR: Mensaje de %d bytes para el puerto %d que no esta soportado!", length, port);
            break;
        }

    }

    // implements -> genielink::os::Task
    void ExampleLorawanTask::run(void* data) {

        // Para envios confirmados desde el dispositivo hacia el servidor
        constexpr unsigned delayInterval = 5;

        uint32_t confirmedCounter = 0;
        uint32_t unconfirmedCounter = 0;
        bool nextConfirmed = true;

        while (1) 
        {

            // Se obtiene acceso esclusivo al estado del dispositivo
            if (lorawanClientAccess.take()) 
            {

                // Se prepara una imagen del estado del dispositivo con el aceso esclusivo a su estado
                genielink::lorawan::ILorawanClient *localLorawanClient = lorawanClient.get();
                bool localConfigured = configured;

                // ya tenemos la imagen del estado y se puede liberar el bloqueo
                lorawanClientAccess.give();

                if (localLorawanClient) 
                {

                    if (localConfigured)
                    {
                        if (nextConfirmed)
                        {

                            nextConfirmed = false;

                            // Se prepara el envio hacia arriba
                            std::string msg { std::to_string(confirmedCounter)};
                            const uint8_t* msg_pointer = reinterpret_cast<const uint8_t*>(msg.c_str());

                            // Envio..
                            genielink::lorawan::LorawanResponse responseCode = localLorawanClient->transmitMessage(msg_pointer, msg.size(), 31, true);

                            // Se reporta el resultado
                            if (responseCode == genielink::lorawan::LorawanResponse::SuccessfulTransmission)
                            {
                                ESP_LOGI(TAG,  "UP-CONFIRMED-OK: '%s'", msg.c_str());
                                confirmedCounter++;
                            } 
                            else 
                            {
                                ESP_LOGE(TAG,  "UP-CONFIRMED-ERROR: '%s'", msg.c_str());
                            }

                        }
                        else {

                            nextConfirmed = true;

                            // Se prepara el envio hacia arriba
                            std::string msg { std::to_string(unconfirmedCounter)};
                            const uint8_t* msg_pointer = reinterpret_cast<const uint8_t*>(msg.c_str());

                            // Envio..
                            genielink::lorawan::LorawanResponse responseCode = localLorawanClient->transmitMessage(msg_pointer, msg.size(), 32, false);

                            // Se reporta el resultado
                            if (responseCode == genielink::lorawan::LorawanResponse::SuccessfulTransmission)
                            {
                                ESP_LOGI(TAG,  "UP-UNCONFIRMED-OK: '%s'", msg.c_str());
                                unconfirmedCounter++;
                            } 
                            else 
                            {
                                ESP_LOGE(TAG,  "UP-UNCONFIRMED-ERROR: '%s'", msg.c_str());
                            }

                        }

                    }
                    else
                    {
                        // Se prepara el envio hacia arriba
                        std::string msg { "boot"};
                        const uint8_t* msg_pointer = reinterpret_cast<const uint8_t*>(msg.c_str());

                        // Envio..
                        genielink::lorawan::LorawanResponse responseCode = localLorawanClient->transmitMessage(msg_pointer, msg.size(), 30);

                        // Se reporta el resultado
                        if (responseCode == genielink::lorawan::LorawanResponse::SuccessfulTransmission)
                        {
                            ESP_LOGI(TAG,  "BOOT-CONFIRMED-OK: '%s'", msg.c_str());
                        } 
                        else 
                        {
                            ESP_LOGE(TAG,  "BOOT-CONFIRMED-ERROR: '%s'", msg.c_str());
                        }

                    }

                } 

            }

            // Una pausa antes del siguiente envio
            ::vTaskDelay(delayInterval * 1000 / portTICK_PERIOD_MS);

        }

    }

    bool ExampleLorawanTask::configureDevice(int id) 
    {
        if (lorawanClientAccess.take()) 
        {
            if (!configured)
            {
                configured = true;
                ESP_LOGI(TAG, "INIT-OK: Configurado el dispositivo con id=%d", id);
            }
            lorawanClientAccess.give();
            return true;
        } 
        return false;
    }

    bool ExampleLorawanTask::receivedConfirmedCounter(int counter) 
    {
        if (lorawanClientAccess.take()) 
        {
            bool localConfigured = configured;
            lorawanClientAccess.give();
            if (localConfigured)
            {
                counterStatistics.updateConfirmed(counter);
                return true;
            }
        } 
        return false;
    }

    bool ExampleLorawanTask::receivedUnconfirmedCounter(int counter) 
    {
        if (lorawanClientAccess.take()) 
        {
            bool localConfigured = configured;
            lorawanClientAccess.give();
            if (localConfigured)
            {
                counterStatistics.updateUnconfirmed(counter);
                return true;
            }
        } 
        return false;
    }

}