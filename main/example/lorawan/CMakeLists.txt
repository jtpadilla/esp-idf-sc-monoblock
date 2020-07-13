idf_component_register(
    SRCS 
        "main.cpp" 
        "example/lorawan/CounterStatistics.cpp" 
        "example/lorawan/CounterStatistics.h" 
        "example/lorawan/SCExampleLorawanTask.cpp" 
        "example/lorawan/SCExampleLorawanTask.h" 
        "example/lorawan/SCExampleLauncher.cpp" 
        "example/lorawan/SCExampleLauncher.h" 
    REQUIRES os lorawan
       
)