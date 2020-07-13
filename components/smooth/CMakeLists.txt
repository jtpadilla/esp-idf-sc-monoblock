
set(COMPONENT_SRCDIRS

    "src/core"
    "src/core/filesystem"
    "src/core/io"
    "src/core/io/i2c"
    "src/core/io/spi"
    "src/core/ipc"
    "src/core/json"
    "src/core/logging"
    "src/core/network"
    "src/core/sntp"
    "src/core/timer"
    "src/core/util"

    "src/application/display"
    "src/application/hash"
    "src/application/io/i2c"
    "src/application/io/spi"
    "src/application/io/wiegand"
    "src/application/network/http"
    "src/application/network/http/regular"
    "src/application/network/http/regular/responses"
    "src/application/network/http/websocket"
    "src/application/network/http/websocket/responses"
    "src/application/network/mqtt"
    "src/application/network/mqtt/packet"
    "src/application/network/mqtt/state"
    "src/application/rgb_led"
    "src/application/security"


)
set(COMPONENT_ADD_INCLUDEDIRS
    "include"
)

set(COMPONENT_REQUIRES
    nvs_flash sdmmc vfs fatfs pthread libsodium mbedtls fmt nlohmann_json
)

register_component()

if (IDF_VER STRGREATER_EQUAL "v4.0")
    target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-expansion-to-defined)
endif()
