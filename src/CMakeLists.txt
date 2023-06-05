set(HEADER_LIST "${PROJECT_SOURCE_DIR}/include/desyl/desyllib.hpp")

add_library(desyl_lib desyllib.cpp grammar.cpp synthesis.cpp ${HEADER_LIST})
target_link_libraries(desyl_lib PRIVATE foonathan::lexy)

target_include_directories(desyl_lib PUBLIC ../include)
target_include_directories(desyl_lib PRIVATE .)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)
