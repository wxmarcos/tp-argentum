target_sources(taller_client
    PRIVATE
    # .cpp files
    main.cpp
    ui/client_app.cpp
    net/client_sender.cpp
    net/client_receiver.cpp
    net/server_connection.cpp
    protocol/protocol.cpp
    game/client_game_state.cpp
    input/input_handler.cpp
    render/world_renderer.cpp
    audio/audio_manager.cpp
    
    PUBLIC
    # .h files
    ui/client_app.h
    net/client_sender.h
    net/client_receiver.h
    net/server_connection.h
    protocol/protocol.h
    protocol/protocol_defs.h
    protocol/client_command.h
    protocol/game_update.h
    game/client_game_state.h
    input/input_handler.h
    render/world_renderer.h
    audio/audio_manager.h
    config/client_config.h
    )

target_include_directories(taller_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
