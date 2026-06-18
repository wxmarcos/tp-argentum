target_sources(taller_server PRIVATE
    persistence_loader.cpp
    persistence_record_mapper.cpp
    PUBLIC
    persistence_loader.h
    persistence_index_record.h
    persistence_record_mapper.h
    persistence_record.h
)
