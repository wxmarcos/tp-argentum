target_sources(taller_server
    PRIVATE
        main.cpp
    PUBLIC
        server.h
)

target_include_directories(taller_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(acceptor)
add_subdirectory(client)
add_subdirectory(config)
add_subdirectory(game)
add_subdirectory(network)
