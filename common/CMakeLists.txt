target_sources(taller_common
    PRIVATE
    # .cpp files
    PUBLIC
    # .h files
    thread.h
    queue.h
    sender.h
    protocol_defs.h
    )

add_subdirectory(network)
add_subdirectory(command)
add_subdirectory(snapshot)
target_include_directories(taller_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
