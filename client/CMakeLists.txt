target_sources(taller_client
    PRIVATE
    main.cpp
    ui/client_app.cpp
    net/client_sender.cpp
    net/client_receiver.cpp
    net/server_connection.cpp
    game/client_game_state.cpp
    input/input_handler.cpp
    render/world_renderer.cpp
    audio/audio_manager.cpp
    protocol/wire_reader.cpp
    config/client_config.cpp

    PUBLIC
    ui/client_app.h
    net/client_sender.h
    net/client_receiver.h
    net/server_connection.h
    protocol/game_update.h
    game/client_game_state.h
    input/input_handler.h
    render/world_renderer.h
    audio/audio_manager.h
    config/client_config.h
    protocol/game_update.h
    protocol/wire_reader.h
    )

target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
