cmake_minimum_required(VERSION 3.5)
project(FreeStun)

set(SDL2_INCLUDE_DIR /usr/include/SDL2/)
set(SDL2_LIBRARY /usr/lib/x86_64-linux-gnu/libSDL2.so)
set(SDL2_NET_LIBRARY /usr/lib/x86_64-linux-gnu/libSDL2_net.so)

include_directories(${SDL2_INCLUDE_DIR})

set(SOURCE_FILES freeStunTest.c)

add_executable(FreeStun ${SOURCE_FILES})
target_link_libraries(FreeStun ${SDL2_LIBRARY})
target_link_libraries(FreeStun ${SDL2_NET_LIBRARY})
target_link_libraries(FreeStun "freestun")