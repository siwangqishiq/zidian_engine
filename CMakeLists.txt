cmake_minimum_required(VERSION 3.10)

project(app CXX)

# for dll import use case 
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


include_directories(include)

file(GLOB_RECURSE ZIDIAN_SRCS CONFIGURE_DEPENDS "src/*.cpp" "src/*.h" "include/*.h")
file(GLOB_RECURSE APP_SRCS CONFIGURE_DEPENDS "sandbox/*.cpp" "sandbox/*.h")

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/windows/libglfw3.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/vender/glfw/include
)
target_include_directories(glfw3 INTERFACE
    ${CMAKE_SOURCE_DIR}/vender/glfw/include
)

add_executable(${PROJECT_NAME} 
    ${APP_SRCS} 
    ${ZIDIAN_SRCS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
