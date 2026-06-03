target_sources(taller_server
    PRIVATE
        main.cpp
    PUBLIC
        server.h
)

target_include_directories(taller_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/acceptor
    ${CMAKE_CURRENT_SOURCE_DIR}/client
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/game
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/persistence
)

add_subdirectory(acceptor)
add_subdirectory(client)
add_subdirectory(config)
add_subdirectory(game)
add_subdirectory(network)
add_subdirectory(persistence)
