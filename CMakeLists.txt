cmake_minimum_required(VERSION 3.10)

project(app CXX)

set(app "app")

# for dll import use case 
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "This linux os")
    set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl -lX11 -latomic")
    set (CMAKE_CXX_FLAGS -pthread)
endif()

if(APPLE)
    set (CMAKE_CXX_FLAGS "-std=c++14") 
    set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -framework CoreFoundation -framework CoreAudio -framework AudioToolbox")
    message(STATUS "This macos os")
endif()

include_directories(include)
# include_directories(vender/glfw/include)

file(GLOB_RECURSE ZIDIAN_SRCS "src/*.cpp" "src/*.h")
file(GLOB_RECURSE APP_SRCS "sandbox/*.cpp" "sandbox/*.h")

add_executable(${PROJECT_NAME} ${APP_SRCS} ${ZIDIAN_SRCS})
