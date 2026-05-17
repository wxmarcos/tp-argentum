target_sources(taller_common
    PRIVATE
    # .cpp files
    receiver.cpp
    PUBLIC
    # .h files
    thread.h
    queue.h
    receiver.h
    sender.h
    )

add_subdirectory(network)
