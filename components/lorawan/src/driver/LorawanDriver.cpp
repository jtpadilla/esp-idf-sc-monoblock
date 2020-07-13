
#include <cstring>
#include <memory>

#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lmic/lmic.h"
#include "LorawanDriver.h"
#include "LorawanLogging.h"

////////////////////////////////////
// Network parameters
////////////////////////////////////

static uint8_t global_app_eui[8];
static uint8_t global_app_key[16];
static uint8_t global_dev_eui[8];

// This EUI must be in little-endian format, so least-significant-byte first.
// When copying an EUI from ttnctl output, this means to reverse the bytes.
// For TTN issued EUIs the last bytes should be 0xD5, 0xB3, 0x70.
// The order is swapped in provisioning_decode_keys().
void os_getArtEui (u1_t* buf)
{
    memcpy(buf, global_app_eui, 8);
}

// This key should be in big endian format (or, since it is not really a number
// but a block of memory, endianness does not really apply). In practice, a key
// taken from ttnctl can be copied as-is.
void os_getDevKey (u1_t* buf)
{
    memcpy(buf, global_app_key, 16);
}

// This should also be in little endian format, see above.
void os_getDevEui (u1_t* buf)
{
    memcpy(buf, global_dev_eui, 8);
}

namespace genielink::lorawan::driver
{

#if LMIC_ENABLE_event_logging
    static LorawanLogging* logging;
#endif

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    // Comunicacion con LMIC [INICIO]
    //
    // El driver le envia los comandos a LMIC invocando sus funciones y
    // espera a que LMIC le notifique el resultado mediante llamadas
    // asincronas a las callbacks.
    // 
    // A su vez las callback tiene que retornar el resultado que han 
    // recibido al driver para lo cual utilizaran una cola ya que 
    // el driver se ha quedado esperando la respuesta en ella.
    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    // Antes de invocar una funcion del LMIC el driver marca que es lo
    // que esta esperando para que las callbacks sepan quien fue
    // el responsable de la llamada.
    enum class WaitingReason
    {
        None,
        ForJoin,
        ForTransmission
    };

    // Identifica el tipo de evento
    enum class LorawanEvent {
        None,
        JoinCompleted,
        JoinFailed,
        MessageReceived,
        TransmissionCompleted,
        TransmissionFailed
    };

    // Las callbacks que son invocadas por LMIC inyectaran este evento 
    // por la cola en la que esta esperando el driver la respuesta.
    struct LmicEvent {

        LmicEvent(LorawanEvent ev = LorawanEvent::None): event(ev) {
        }

        LorawanEvent event;
        uint8_t port;
        const uint8_t* message;
        size_t messageSize;
        
    };

    // Cola para obtener los resultados del LMIC.
    //   Driver->LMIC->Callbacks->[QUEUE]->Driver
    static QueueHandle_t lmicEventQueue = nullptr;

    // Para que el callback sepa quien ha generado la peticion 
    //   Driver(set-Reason)->LMIC->Calbacks(query-Reason)->Driver
    static WaitingReason waitingReason = WaitingReason::None;

    // Las tres callbacks que utiliza el LMIC
    static void eventCallback(void* userData, ev_t event);
    static void messageReceivedCallback(void *userData, uint8_t port, const uint8_t *message, size_t messageSize);
    static void messageTransmittedCallback(void *userData, int success);

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
    // Comunicacion con LMIC [FINAL]
    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    static const char *TAG = "genielink::lorawan::driver::lorawandriver";

    // La unica funcion de esta variable es para asegurarse que solo se puede crear un driver.
    static LorawanDriver* ttnInstance;

    // Instancia el driver (una sola vez) y antes de crear la instancia 
    // realiza todas las inicializaciones globales.
    std::unique_ptr<LorawanDriver> LorawanDriver::instantiate(const LorawanDevice lorawanDevice, const LorawanParameters& lorawanParameters) {

        // El proyecto tiene que estar correctamente configurado
    #if defined(TTN_IS_DISABLED)
        ESP_LOGE(TAG, "TTN is disabled. Configure a frequency plan using 'make menuconfig'");
        ASSERT(0);
    #endif

        // Se verifica que solo se crea una unica instancia
        ESP_LOGD(TAG, "Se verifica que no hay otra instancia iniciada.");
        ASSERT(ttnInstance == nullptr);

        // Los parametros para conectarse a la red Lora se colocan en las variables golables de LMIC
        ESP_LOGD(TAG, "Se instalan la informacion para el acceso a la red Lorawan.");
        std::memcpy(::global_app_eui, &lorawanParameters.appEui, sizeof(lorawanParameters.appEui));
        std::memcpy(::global_app_key, &lorawanParameters.appKey, sizeof(lorawanParameters.appKey));
        std::memcpy(::global_dev_eui, &lorawanParameters.devEui, sizeof(lorawanParameters.devEui));

        // Se seleccionna los pins para el device que nos han indicado
        std::unique_ptr<LorawanPins> lorawanPins(LorawanPins::instantiate(lorawanDevice));
        ESP_LOGD(TAG, "Se ha cargado la definion para la placa '%s'", lorawanPins->get_name().c_str());

        // Se configuran los pins en el HAL
        ESP_LOGD(TAG, "Se configuran los PINs del chip.");
        ::lorawanhal.configurePins(lorawanPins.get());

    #if LMIC_ENABLE_event_logging
        logging = TTNLogging::initInstance();
    #endif

        // Se registra el callback que recibira los eventos desde LMIC
        ESP_LOGD(TAG, "Se registra el retrollamada para recibir eventos desde LMIC.");
        ::LMIC_registerEventCb(eventCallback, nullptr);

        // Se registra el callback que recibira las recepciones desde LMIC
        ESP_LOGD(TAG, "Se registra el retrollamada para recibir mensajes desde LMIC.");
        ::LMIC_registerRxMessageCb(messageReceivedCallback, nullptr);

        // Se inicializa LMIC
        ESP_LOGD(TAG, "Se inicializa LMIC.");
        ::os_init_ex(nullptr);
        //reset();

        // Se inicializa la cola por donde los callbacks enviaran los eventos a la presente clase
        ESP_LOGD(TAG, "Se crea una cola para recibir las comunicaciones desde las retrollamadas invoicadas por LMIC.");
        lmicEventQueue = ::xQueueCreate(4, sizeof(LmicEvent));
        ASSERT(lmicEventQueue != nullptr);

        // Se inicia la terea para LMIC
        ESP_LOGD(TAG, "Se inicia la tarea del LMIC.");
        ::lorawanhal.startLMICTask();
        
        // Se crea la instancia
        ESP_LOGD(TAG, "Se instancia el driver.");
        return std::make_unique<LorawanDriver>();

    }

    LorawanDriver::LorawanDriver():
        listener {nullptr}
    {
        reset();
    }

    LorawanDriver::~LorawanDriver()
    {
    }

    void LorawanDriver::installListener(ILorawanListener *lorawanListener) {
        if (listener) {
            ESP_LOGE(TAG, "El listener ya esta instslado!");
        } else {
            ESP_LOGD(TAG, "Instalado el listener para mensajes bajantes");
            listener = lorawanListener;
        }
    }

    void LorawanDriver::reset()
    {
        ::lorawanhal.enterCriticalSection();
        ::LMIC_reset();
        waitingReason = WaitingReason::None;
        if (lmicEventQueue != nullptr)
        {
            ::xQueueReset(lmicEventQueue);
        }
        ::lorawanhal.leaveCriticalSection();
    }

    bool LorawanDriver::join()
    {
        lorawanhal.enterCriticalSection();
        waitingReason = WaitingReason::ForJoin;
        ::LMIC_startJoining();
        lorawanhal.wakeUp();
        lorawanhal.leaveCriticalSection();

        LmicEvent event;
        ::xQueueReceive(lmicEventQueue, &event, portMAX_DELAY);
        return event.event == LorawanEvent::JoinCompleted;
    }

    genielink::lorawan::LorawanResponse LorawanDriver::transmitMessage(const uint8_t *payload, size_t length, port_t port, bool confirm)
    {
        ::lorawanhal.enterCriticalSection();
        if (waitingReason != WaitingReason::None || (LMIC.opmode & OP_TXRXPEND) != 0)
        {
            ::lorawanhal.leaveCriticalSection();
            return LorawanResponse::ErrorTransmissionFailed;
        }

        waitingReason = WaitingReason::ForTransmission;
        ::LMIC.client.txMessageCb = messageTransmittedCallback;
        ::LMIC.client.txMessageUserData = nullptr;
        ::LMIC_setTxData2(port, (xref2u1_t)payload, length, confirm);
        ::lorawanhal.wakeUp();
        ::lorawanhal.leaveCriticalSection();

        while (true)
        {
            LmicEvent result;
            ::xQueueReceive(lmicEventQueue, &result, portMAX_DELAY);

            switch (result.event)
            {
                case LorawanEvent::MessageReceived:
                    if (listener) {
                        //ESP_LOGD(TAG, "RX size(%d), port(%d", result.messageSize, result.port);
                        listener->lorawanMessageReceived(result.message, result.messageSize, result.port);
                    } else {
                        ESP_LOGE(TAG, "Unavailable listener");
                    }
                    break;

                case LorawanEvent::TransmissionCompleted:
                    return genielink::lorawan::LorawanResponse::SuccessfulTransmission;

                case LorawanEvent::TransmissionFailed:
                    return genielink::lorawan::LorawanResponse::ErrorTransmissionFailed;

                default:
                    ASSERT(0);
            }
        }
    }

    void LorawanDriver::setRSSICal(int8_t rssiCal)
    {
        ::lorawanhal.rssiCal = rssiCal;
    }


    // --- Callbacks ---

#if CONFIG_LOG_DEFAULT_LEVEL >= 3 || LMIC_ENABLE_event_logging
    const char *eventNames[] = { LMIC_EVENT_NAME_TABLE__INIT };
#endif


    // Called by LMIC when an LMIC event (join, join failed, reset etc.) occurs
    void eventCallback(void* userData, ev_t event)
    {
#if LMIC_ENABLE_event_logging
        logging->logEvent(event, eventNames[event], 0);
#elif CONFIG_LOG_DEFAULT_LEVEL >= 3
        ESP_LOGD(TAG, "event %s", eventNames[event]);
#endif

        LorawanEvent lorawanEvent = LorawanEvent::None;

        if (waitingReason == WaitingReason::ForJoin)
        {
            if (event == EV_JOINED)
            {
                lorawanEvent = LorawanEvent::JoinCompleted;
            }
            else if (event == EV_REJOIN_FAILED || event == EV_RESET)
            {
                lorawanEvent = LorawanEvent::JoinFailed;
            }
        }

        if (lorawanEvent == LorawanEvent::None)
            return;

        LmicEvent result(lorawanEvent);
        waitingReason = WaitingReason::None;
        ::xQueueSend(lmicEventQueue, &result, pdMS_TO_TICKS(100));
    }

    // Sera llamado por LMIC cuando se reciba un mensaje
    void messageReceivedCallback(void *userData, uint8_t port, const uint8_t *message, size_t nMessage)
    {
        LmicEvent result(LorawanEvent::MessageReceived);
        result.port = port;
        result.message = message;
        result.messageSize = nMessage;
        ::xQueueSend(lmicEventQueue, &result, pdMS_TO_TICKS(100));
    }

    // sera llamado por LMIC cuando un mensaje se ha transmitido (o la transmision ha fallado)
    void messageTransmittedCallback(void *userData, int success)
    {
        waitingReason = WaitingReason::None;
        LmicEvent result(success ? LorawanEvent::TransmissionCompleted : LorawanEvent::TransmissionFailed);
        ::xQueueSend(lmicEventQueue, &result, pdMS_TO_TICKS(100));
    }

}