
#include "string"

#include "esp_log.h"

#include "LorawanParameters.h"
#include "LorawanUtil.h"
#include "LorawanClient.h"
#include "LorawanLauncher.h"
#include "LorawanDevice.h"
#include "SCExampleLorawanTask.h"


constexpr char devEui[] = "004CFEED74AD2FA6";
constexpr char appEui[] = "70B3D57ED00306F7";
constexpr char appKey[] = "8214F6A2800C9FCD9B26BBE28D5CD057";

namespace example::lorawan
{

    static const char* JOINTASK_TAG = "example::lorawan::jointask";

    using namespace genielink::lorawan;

    class JoinTask: public genielink::os::Task
    {

        LorawanDevice lorawanDevice;
        LorawanParameters& lorawanParameters;
        ILorawanActivator& lorawanActivator;

        public:

            JoinTask(LorawanDevice lorawanDeviceParam, LorawanParameters& lorawanParametersParam, ILorawanActivator& lorawanActivatorParam):
                genielink::os::Task{"jointask", 10000}, lorawanDevice {lorawanDeviceParam}, lorawanParameters {lorawanParametersParam}, lorawanActivator {lorawanActivatorParam}
            {
            }

            void run(void* data) {
                ESP_LOGI(JOINTASK_TAG, "Se establece la conexion y se activa el cliente lorawan");
                LorawanLauncher::launch(lorawanDevice, lorawanParameters, lorawanActivator);
            }

    };

    extern void launch();

}