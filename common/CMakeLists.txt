target_sources(taller_common
    PRIVATE
    # .cpp files
    receiver.cpp
    command.cpp
    snapshot.cpp
    PUBLIC
    # .h files
    thread.h
    queue.h
    receiver.h
    sender.h
    snapshot.h
    command.h
    )

add_subdirectory(network)

target_include_directories(taller_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
