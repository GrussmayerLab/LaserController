add_library(mcp4922 INTERFACE)

target_include_directories(mcp4922 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_sources(mcp4922 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/mcp4922.cpp)

# Pull in pico libraries that we need
target_link_libraries(mcp4922 INTERFACE pico_stdlib hardware_spi hardware_gpio)