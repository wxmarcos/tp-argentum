target_sources(taller_server PRIVATE
    client_handler.cpp
    monitor_clients.cpp
    quit_listener.cpp
    PUBLIC
    client_handler.h
    monitor_clients.h
    quit_listener.h
)
