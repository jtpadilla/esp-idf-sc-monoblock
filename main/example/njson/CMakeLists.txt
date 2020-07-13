idf_component_register(
    SRCS 
        "main.cpp" 
        "example/njson/ExampleNJson.cpp" 
        "example/njson/ExampleNJson.h" 

        REQUIRES nlohmann_json fmt
        
)