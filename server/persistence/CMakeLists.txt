target_sources(taller_server PRIVATE
    persistence_worker.cpp
    persistence_task_factory.cpp
    PUBLIC
    persistence_worker.h
    persistence_task_factory.h

)

add_subdirectory(clan)
add_subdirectory(players)
