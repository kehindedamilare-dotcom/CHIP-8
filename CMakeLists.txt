cmake_minimum_required(VERSION 4.0)
project(CHIP_8 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3-shared)

add_executable(CHIP_8 main.cpp
        chip8.h
)
target_link_libraries(CHIP_8 PRIVATE SDL3::SDL3)

if(WIN32)
    add_custom_command(TARGET CHIP_8 POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:CHIP_8>
            VERBATIM
    )
endif()