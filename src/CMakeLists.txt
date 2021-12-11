set(C_STANDARD 99)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.h
    ${CMAKE_CURRENT_SOURCE_DIR}/threads.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/threads.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tree.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tree.h
)


find_package(IMGUI REQUIRED)
find_package(SDL2 REQUIRED)
include(FindOpenGL)

set(CFLAGS
    -Wall -Wextra -pedantic
    -Wconversion -Wshadow -Wstrict-aliasing
    -Winit-self -Wcast-align -Wpointer-arith
    -Wmissing-declarations -Wmissing-include-dirs
    -Wno-unused-parameter -Wuninitialized
)

add_library(core OBJECT ${SOURCES})

target_include_directories(core PUBLIC ${IMGUI_INCLUDE_DIRS})
target_include_directories(core PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(core PUBLIC ${OPENGL_INCLUDE_DIRS})

target_compile_options(core PUBLIC ${CFLAGS})

add_executable(${CMAKE_PROJECT_NAME} $<TARGET_OBJECTS:core>)

target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${IMGUI_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${SDL2_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${CMAKE_DL_LIBS})

install(TARGETS ${CMAKE_PROJECT_NAME})