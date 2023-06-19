set(HEADER_LIST "${PROJECT_SOURCE_DIR}/include/desyl/desyllib.hpp")

add_library(desyl_lib desyllib.cpp synthesis.cpp rules.cpp)
add_subdirectory(ast)
add_subdirectory(parser)
add_subdirectory(context)
target_link_libraries(desyl_lib PRIVATE ast PRIVATE parser PRIVATE context)

target_include_directories(desyl_lib PUBLIC ../include)
target_include_directories(desyl_lib PRIVATE . PRIVATE ./include)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)
