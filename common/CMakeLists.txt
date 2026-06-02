target_sources(taller_common
    PRIVATE
    # .h files
    thread.h
    queue.h
    sender.h
    command/command.h
    snapshot/snapshot.h
    protocol_defs.h
)

add_subdirectory(command)
add_subdirectory(snapshot)
add_subdirectory(network)

target_include_directories(taller_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
