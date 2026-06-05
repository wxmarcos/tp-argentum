target_sources(taller_common
    PRIVATE
    command.cpp
    command_codec.cpp
    packet_helpers.cpp
)

target_include_directories(taller_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
