target_sources(taller_server
    PRIVATE
    # .cpp files
    main.cpp
    client_handler.cpp
    acceptor.cpp
    game_loop.cpp
    config.cpp
    receiver.cpp
    PUBLIC
    # .h files
    client_handler.h
    acceptor.h
    game_loop.h
    config.h
    receiver.h
    )
