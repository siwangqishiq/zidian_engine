cmake_minimum_required(VERSION 3.10)

project(app LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
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

if (MSVC)
    message(STATUS "Using Windows MSVC compiler")
    set_target_properties(glfw3 PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/windows_msvc/glfw3.lib
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/vendor/glfw/include
    )
else()
    message(STATUS "Using Windows msys urt64 compiler")
    set_target_properties(glfw3 PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/windows/libglfw3.a
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/vendor/glfw/include
    )
endif()

# set_target_properties(glfw3 PROPERTIES
#     IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/libglfw3.a
# )

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

# add_compile_options(-Wno-deprecated-declarations)

function(ignore_warning_msvc)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd5045)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4267) 
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4464)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4819)  
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4996) 
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4244)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4099)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4834)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4464)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4514)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4820)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4100)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4365)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4625)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4626)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd5026)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd5027)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4774)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd5204)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4355)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4018)
endfunction()

add_executable(${PROJECT_NAME} 
    ${APP_SRCS} 
    ${ZIDIAN_SRCS}
)

if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    ignore_warning_msvc()
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations")
endif()

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        glfw3
        vulkan
)

target_include_directories(${PROJECT_NAME}
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_dependencies(${PROJECT_NAME} Shaders)


# 导出include目录
# file(GLOB_RECURSE ZIDIAN_HEADERS CONFIGURE_DEPENDS
#     "${CMAKE_SOURCE_DIR}/src/*.h"
# )

# set(EXPORT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/include")

# add_custom_target(ExportHeaders ALL)

# foreach(HEADER ${ZIDIAN_HEADERS})
#     file(RELATIVE_PATH REL_PATH
#         "${CMAKE_SOURCE_DIR}/src"
#         "${HEADER}"
#     )

#     get_filename_component(REL_DIR
#         "${REL_PATH}"
#         DIRECTORY
#     )

#     add_custom_command(TARGET ExportHeaders POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E make_directory
#             "${EXPORT_INCLUDE_DIR}/${REL_DIR}"

#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#             "${HEADER}"
#             "${EXPORT_INCLUDE_DIR}/${REL_PATH}"
#     )
# endforeach()
