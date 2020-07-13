
set(COMPONENT_SRCDIRS
    "src"
    "src/driver"
    "src/driver/lmic"
    "src/driver/aes"
)
set(COMPONENT_ADD_INCLUDEDIRS
    "include"
)

set(COMPONENT_REQUIRES
    mbedtls
)

register_component()

if (IDF_VER STRGREATER_EQUAL "v4.0")
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-expansion-to-defined)
endif()
