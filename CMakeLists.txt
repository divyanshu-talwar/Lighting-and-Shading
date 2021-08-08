cmake_minimum_required(VERSION 3.5)
project(lighting_and_shading)
find_package( OpenGL REQUIRED )
set(CMAKE_MODULE_PATH  ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
include(HelperFunctions)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/bin)

include_directories(
	"ext/glfw/include"
	"ext/glm"
	"ext/glew/include"
        "src"
        )

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(ext/glew/build/cmake)
add_subdirectory(ext/glfw)

set(PLATFORM_LINK )
if (UNIX)
    set(PLATFORM_LINK -ldl)
endif (UNIX)
file(GLOB SOURCE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*")

add_executable(lighting_and_shading ${SOURCE_FILES})
target_link_libraries(lighting_and_shading glfw ${GLFW_LIBRARIES} ${PLATFORM_LINK})
target_link_libraries(lighting_and_shading glew ${GLEW_LIBRARIES} ${PLATFORM_LINK})

pre_build(lighting_and_shading
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res)
