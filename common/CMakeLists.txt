target_sources(taller_common
    PRIVATE
    # .cpp files
    foo.cpp
    PUBLIC
    # .h files
    foo.h
    )

add_subdirectory(network)

target_include_directories(taller_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
