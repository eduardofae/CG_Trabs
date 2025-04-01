cmake_minimum_required (VERSION 2.6)

project (CG_TRABS)

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(IMGUI_DIR ${EXT_DIR}/imgui)

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin)
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set (CMAKE_DEBUG_POSTFIX "_d")

find_package(OpenGL REQUIRED)

if(WIN32)
set(COMMON_LIBS ${OPENGL_LIBRARIES} optimized glfw debug glfw)
elseif (UNIX)
set(COMMON_LIBS ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES})
else()
set(COMMON_LIBS)
endif()
set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

set(RUN_DIR ${PROJECT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} src/main.cpp lib/gl3w.c shaders/openGL.vert shaders/openGL.frag src/loader/loader.cpp src/GUI/GUI.cpp src/utils/matrices.cpp src/utils/printMatrices.cpp src/camera/camera.cpp src/render/openGL.cpp src/render/closeGL.cpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY DEBUG_POSTFIX _d)
target_link_libraries(${PROJECT_NAME} ${COMMON_LIBS})
add_subdirectory("${EXT_DIR}/glfw")
add_library(imgui ${IMGUI_DIR}/imgui.h ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_widgets.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp ${EXT_DIR}/glfw)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw)
include_directories(${EXT_DIR}/stb)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw imgui)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

include_directories( include )
include_directories(lib/glfw/include)

option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
