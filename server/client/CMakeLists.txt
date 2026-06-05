target_sources(taller_server
    PRIVATE
        monitor_clients.cpp
        client_handler.cpp
    PUBLIC
        monitor_clients.h
        client_handler.h
)
