set(CommonAPI_FOUND FALSE)
get_filename_component(CommonAPI_CMAKE_PREFIX ${CMAKE_CURRENT_LIST_DIR} PATH)

function(strip_sysroot PATH OUTPUT_VAR)
    foreach(ROOT_PATH ${CMAKE_FIND_ROOT_PATH})
        get_filename_component(RESOLVED_ROOT_PATH ${ROOT_PATH} REALPATH)
        string(FIND ${PATH} ${RESOLVED_ROOT_PATH} PREFIX_POSITION)
        if(PREFIX_POSITION EQUAL 0)
            string(LENGTH ${RESOLVED_ROOT_PATH} ROOT_PATH_LENGTH)
            string(SUBSTRING ${PATH} ${ROOT_PATH_LENGTH} -1 ${OUTPUT_VAR})
            set(${OUTPUT_VAR} ${${OUTPUT_VAR}} PARENT_SCOPE)
            return()
        endif()
    endforeach()
    set(${OUTPUT_VAR} ${PATH} PARENT_SCOPE)
endfunction()

function(determine_relative_paths)
    set(prefix "/usr")
    set(exec_prefix "/usr")
    set(datadir "/usr/share")
    set(libdir "/usr/lib")
    set(includedir "/usr/include")
    set(cmakeinstalldir "/usr/lib/cmake")

    file(RELATIVE_PATH RELATIVE_INCLUDE_DIR ${cmakeinstalldir} ${includedir})
    file(RELATIVE_PATH RELATIVE_LIB_DIR ${cmakeinstalldir} ${libdir})
    file(RELATIVE_PATH RELATIVE_DATA_DIR ${cmakeinstalldir} ${datadir})

    set(RELATIVE_INCLUDE_DIR ${RELATIVE_INCLUDE_DIR} PARENT_SCOPE)
    set(RELATIVE_LIB_DIR ${RELATIVE_LIB_DIR} PARENT_SCOPE)
    set(RELATIVE_DATA_DIR ${RELATIVE_DATA_DIR} PARENT_SCOPE)
endfunction()

determine_relative_paths()
strip_sysroot(${CommonAPI_CMAKE_PREFIX} CommonAPI_BARE_CMAKE_PREFIX)

find_path(CommonAPI_INCLUDE_DIR CommonAPI/CommonAPI.h HINTS "${CommonAPI_BARE_CMAKE_PREFIX}/${RELATIVE_INCLUDE_DIR}/CommonAPI-2.1" NO_DEFAULT_PATH)
find_library(CommonAPI_LIBRARY CommonAPI HINTS ${CommonAPI_BARE_CMAKE_PREFIX}/${RELATIVE_LIB_DIR} NO_DEFAULT_PATH)
find_file(CommonAPI_GENERATOR_JAR common-api-generator.jar HINTS ${CommonAPI_BARE_CMAKE_PREFIX}/${RELATIVE_DATA_DIR}/CommonAPI-2.1.5 NO_DEFAULT_PATH)

unset(RELATIVE_INCLUDE_DIR)
unset(RELATIVE_LIB_DIR)
unset(RELATIVE_DATA_DIR)
unset(CommonAPI_BARE_CMAKE_PREFIX)

if(CommonAPI_FIND_REQUIRED_GENERATOR AND NOT CommonAPI_GENERATOR_JAR)
    message(FATAL_ERROR "Could not find CommonAPI common-api-generator.jar in ${CommonAPI_BARE_CMAKE_PREFIX}/${RELATIVE_DATA_DIR}/CommonAPI-2.1.5/")
    return()
endif()

if(NOT CommonAPI_INCLUDE_DIR OR NOT CommonAPI_LIBRARY)
    return()
endif()

set(CommonAPI_VERSION 2.1.5)
set(CommonAPI_VERSION_STRING "2.1.5")
set(CommonAPI_INCLUDE_DIRS ${CommonAPI_INCLUDE_DIR})
set(CommonAPI_LIBRARIES ${CommonAPI_LIBRARY})

set(CommonAPI_FOUND TRUE)

include(${CMAKE_CURRENT_LIST_DIR}/CommonAPIMacros.cmake)
