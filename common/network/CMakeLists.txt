# common/network/CMakeLists.txt

target_sources(taller_common
    PRIVATE
     liberror.cpp
     resolver.cpp
     resolvererror.cpp
     socket.cpp
)

target_include_directories(taller_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})