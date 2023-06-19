add_library(synthesis synthesis.cpp rules.cpp)

target_link_libraries(synthesis PRIVATE context)

target_include_directories(synthesis
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE .
)
