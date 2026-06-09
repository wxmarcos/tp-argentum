target_sources(taller_server PRIVATE
    persistence_worker.cpp
    persistence_task_factory.cpp
    persistence_loader.cpp
    persistence_record_mapper.cpp
    PUBLIC
    persistence_worker.h
    persistence_task_factory.h
    persistence_loader.h
    persistence_record_mapper.h
    persistence_record.h
)
