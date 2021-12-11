#
# SDL2_INCLUDE_DIRS
# SDL2_LIBRARIES
# SDL2_CFLAGS

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(_SDL2 sdl2)

    if (BUILD_STATIC AND NOT _SDL2_FOUND)
         message(FATAL_ERROR "Cannot find static build information")
    endif()
endif()

if (_SDL2_FOUND) # we can rely on pkg-config
    if (NOT BUILD_STATIC)
        set(SDL2_LIBRARIES ${_SDL2_LIBRARIES})
        set(SDL2_INCLUDE_DIRS ${_SDL2_INCLUDE_DIRS})
        set(SDL2_CFLAGS ${_SDL2_CFLAGS_OTHER})
    else()
        set(SDL2_LIBRARIES ${_SDL2_STATIC_LIBRARIES})
        set(SDL2_INCLUDE_DIRS ${_SDL2_STATIC_INCLUDE_DIRS})
        set(SDL2_CFLAGS ${_SDL2_STATIC_CFLAGS_OTHER})
    endif()
else()
    find_path(SDL2_INC
        NAMES libsdl2
        HINTS
            ENV sdl2Path
        PATHS
            /usr/include/SDL2 /usr/local/include/SDL2
    )

    find_library(SDL2_LIB
        NAMES libSDL2
        HINTS
            ENV sdl2Path
        PATHS
            /usr/lib /usr/local/lib
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(SDL2 DEFAULT_MSG SDL2_LIB SDL2_INC)
    mark_as_advanced(SDL2_INC SDL2_LIB)

    if(SDL2_FOUND)
        set(SDL2_INCLUDE_DIRS ${SDL2_INC})
        set(SDL2_LIBRARIES ${SDL2_LIB})
        if (BUILD_STATIC)
            set(SDL2_LIBRARIES ${SDL2_LIBRARIES} ${_SDL2_STATIC_LIBRARIES})
        endif()
    endif()
endif()