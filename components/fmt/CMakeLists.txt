
set(COMPONENT_SRCDIRS
  "src"
)
set(COMPONENT_ADD_INCLUDEDIRS
  "include"
)

register_component()

if (IDF_VER STRGREATER_EQUAL "v4.0")
  target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-expansion-to-defined)
endif()
