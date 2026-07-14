cmake_minimum_required(VERSION 3.10)

project(app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for dll import use case 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Shader目录
set(SHADER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/shaders)
# SPIR-V输出目录
set(SHADER_BINARY_DIR ${CMAKE_SOURCE_DIR}/assets/shaders)
# glslangValidator
set(GLSLANG_VALIDATOR
    ${CMAKE_SOURCE_DIR}/tools/glsl/glslang.exe)
# 搜索所有Shader
file(GLOB_RECURSE GLSL_FILES
    CONFIGURE_DEPENDS
    "${SHADER_SOURCE_DIR}/*.vert"
    "${SHADER_SOURCE_DIR}/*.frag"
)

set(SPV_FILES)
foreach(GLSL ${GLSL_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)

    set(SPV ${SHADER_BINARY_DIR}/${FILE_NAME}.spv)
    add_custom_command(
        OUTPUT ${SPV}
        COMMAND
            ${GLSLANG_VALIDATOR}
            -V
            ${GLSL}
            -o
            ${SPV}
        DEPENDS
            ${GLSL}
        COMMENT "Compiling Shader ${FILE_NAME}"
        VERBATIM
    )

    list(APPEND SPV_FILES ${SPV})
endforeach()

add_custom_target(
    Shaders
    ALL
    DEPENDS
    ${SPV_FILES}
)

file(GLOB_RECURSE ZIDIAN_SRCS CONFIGURE_DEPENDS 
    "src/*.cpp" 
    "src/*.h" 
    "include/*.h"
)

file(GLOB_RECURSE APP_SRCS CONFIGURE_DEPENDS 
    "sandbox/*.cpp" 
    "sandbox/*.h"
)

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/libglfw3.a
)
target_include_directories(glfw3 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include
)

add_library(vulkan SHARED IMPORTED)
set_target_properties(vulkan PROPERTIES
    IMPORTED_IMPLIB
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/vulkan-1.lib
)
target_include_directories(vulkan INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vk
)

add_compile_options(-Wno-deprecated-declarations)

add_executable(${PROJECT_NAME} 
    ${APP_SRCS} 
    ${ZIDIAN_SRCS}
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        glfw3
        vulkan
)

target_include_directories(${PROJECT_NAME}
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_dependencies(${PROJECT_NAME} Shaders)
