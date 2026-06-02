target_sources(taller_server PRIVATE
    persistence_worker.cpp
    persistence_task_factory.cpp
    persistance_loader.cpp
    PUBLIC
    persistence_worker.h
    persistence_task_factory.h
    persistance_loader.h
)
