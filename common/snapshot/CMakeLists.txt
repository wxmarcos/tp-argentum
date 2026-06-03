target_sources(taller_common
    PRIVATE
    snapshot.cpp
    snapshot_codec.cpp
)

target_include_directories(taller_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
