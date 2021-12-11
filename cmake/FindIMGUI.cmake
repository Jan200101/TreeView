#
# IMGUI_INCLUDE_DIRS
# IMGUI_CFLAGS

set(IMGUI_PATH ${PROJECT_SOURCE_DIR}/deps/imgui)
if(NOT EXISTS ${IMGUI_PATH}/imgui.h)
    find_package(Git QUIET)

    if (Git_FOUND)
        message(STATUS "Cloning IMGUI to ${IMGUI_PATH}")
        execute_process(COMMAND
            ${GIT_EXECUTABLE} submodule update --init --recursive -- ${dir}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_STDOUT
            ERROR_VARIABLE GIT_STDERR
            COMMAND_ERROR_IS_FATAL ANY)
    else()
        message(FATAL_ERROR "Unable to find imgui.\nMake sure to run `git submodule update --init --recursive`")
    endif()

endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(IMGUI DEFAULT_MSG IMGUI_PATH)
mark_as_advanced(IMGUI_PATH)

find_package(SDL2 REQUIRED)

add_library(
    imgui
    STATIC
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imgui.h
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp

    ${IMGUI_PATH}/backends/imgui_impl_sdl.cpp
    ${IMGUI_PATH}/backends/imgui_impl_sdl.h
    ${IMGUI_PATH}/backends/imgui_impl_opengl2.cpp
    ${IMGUI_PATH}/backends/imgui_impl_opengl2.h
)

target_include_directories(imgui PRIVATE ${IMGUI_PATH} ${IMGUI_PATH}/backends)
target_include_directories(imgui PUBLIC ${SDL2_INCLUDE_DIRS})

if(IMGUI_FOUND)
    set(IMGUI_INCLUDE_DIRS ${IMGUI_PATH} ${IMGUI_PATH}/backends)
    set(IMGUI_LIBRARIES imgui)
endif()