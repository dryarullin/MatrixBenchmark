# version: 0.16.0-dev

include(CMakeParseArguments)

function(conan_cmake_settings result)

    message(STATUS "Conan: Automatic detection of conan settings from cmake")

    conan_parse_arguments(${ARGV})

    if(ARGUMENTS_BUILD_TYPE)
        set(_CONAN_SETTING_BUILD_TYPE ${ARGUMENTS_BUILD_TYPE})
    elseif(CMAKE_BUILD_TYPE)
        set(_CONAN_SETTING_BUILD_TYPE ${CMAKE_BUILD_TYPE})
    else()
        message(FATAL_ERROR "Please specify in command line CMAKE_BUILD_TYPE (-DCMAKE_BUILD_TYPE=Release)")
    endif()

    # If profile is defined it is used
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND ARGUMENTS_DEBUG_PROFILE)
        set(_APPLIED_PROFILES ${ARGUMENTS_DEBUG_PROFILE})
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release" AND ARGUMENTS_RELEASE_PROFILE)
        set(_APPLIED_PROFILES ${ARGUMENTS_RELEASE_PROFILE})
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" AND ARGUMENTS_RELWITHDEBINFO_PROFILE)
        set(_APPLIED_PROFILES ${ARGUMENTS_RELWITHDEBINFO_PROFILE})
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" AND ARGUMENTS_MINSIZEREL_PROFILE)
        set(_APPLIED_PROFILES ${ARGUMENTS_MINSIZEREL_PROFILE})
    elseif(ARGUMENTS_PROFILE)
        set(_APPLIED_PROFILES ${ARGUMENTS_PROFILE})
    endif()

    foreach(ARG ${_APPLIED_PROFILES})
        set(_SETTINGS ${_SETTINGS} -pr=${ARG})
    endforeach()
    foreach(ARG ${ARGUMENTS_PROFILE_BUILD})
        conan_check(VERSION 1.24.0 REQUIRED DETECT_QUIET)
        set(_SETTINGS ${_SETTINGS} -pr:b=${ARG})
    endforeach()
    
    message(STATUS "Conan: Settings= ${_SETTINGS}")

    set(${result} ${_SETTINGS} PARENT_SCOPE)
endfunction()

macro(conan_parse_arguments)
  set(options BASIC_SETUP CMAKE_TARGETS UPDATE KEEP_RPATHS NO_LOAD NO_OUTPUT_DIRS OUTPUT_QUIET NO_IMPORTS SKIP_STD)
  set(oneValueArgs CONANFILE  ARCH BUILD_TYPE INSTALL_FOLDER CONAN_COMMAND)
  set(multiValueArgs DEBUG_PROFILE RELEASE_PROFILE RELWITHDEBINFO_PROFILE MINSIZEREL_PROFILE
                     PROFILE REQUIRES OPTIONS IMPORTS SETTINGS BUILD ENV GENERATORS PROFILE_AUTO
                     INSTALL_ARGS CONFIGURATION_TYPES PROFILE_BUILD)
  cmake_parse_arguments(ARGUMENTS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
endmacro()

function(conan_cmake_install)
    conan_parse_arguments(${ARGV})

    set(ARGUMENTS_GENERATORS ${ARGUMENTS_GENERATORS} cmake_find_package_multi)

    if(ARGUMENTS_CONAN_COMMAND)
       set(CONAN_CMD ${ARGUMENTS_CONAN_COMMAND})
    else()
        conan_check(REQUIRED)
    endif()
    if(ARGUMENTS_CONANFILE)
      if(IS_ABSOLUTE ${ARGUMENTS_CONANFILE})
          set(CONANFILE ${ARGUMENTS_CONANFILE})
      else()
          set(CONANFILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARGUMENTS_CONANFILE})
      endif()
    else()
      set(CONANFILE ".")
    endif()
    foreach(ARG ${ARGUMENTS_GENERATORS})
        set(CONAN_GENERATORS ${CONAN_GENERATORS} -g=${ARG})
    endforeach()
    set(conan_args install ${CONANFILE} ${settings} ${CONAN_GENERATORS})

    string (REPLACE ";" " " _conan_args "${conan_args}")
    message(STATUS "Conan executing: ${CONAN_CMD} ${_conan_args}")

    execute_process(COMMAND ${CONAN_CMD} ${conan_args}
                    RESULT_VARIABLE return_code
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    if(NOT "${return_code}" STREQUAL "0")
      message(FATAL_ERROR "Conan install failed='${return_code}'")
    endif()

endfunction()

function(conan_cmake_setup_conanfile)
    conan_parse_arguments(${ARGV})
    get_filename_component(_CONANFILE_NAME ${ARGUMENTS_CONANFILE} NAME)
    # configure_file will make sure cmake re-runs when conanfile is updated
    configure_file(${ARGUMENTS_CONANFILE} ${CMAKE_CURRENT_BINARY_DIR}/${_CONANFILE_NAME}.junk COPYONLY)
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${_CONANFILE_NAME}.junk)
endfunction()

macro(conan_cmake_run)
    conan_parse_arguments(${ARGV})
    
    if(ARGUMENTS_CONFIGURATION_TYPES AND NOT CMAKE_CONFIGURATION_TYPES)
        message(WARNING "CONFIGURATION_TYPES should only be specified for multi-configuration generators")
    elseif(ARGUMENTS_CONFIGURATION_TYPES AND ARGUMENTS_BUILD_TYPE)
        message(WARNING "CONFIGURATION_TYPES and BUILD_TYPE arguments should not be defined at the same time.")
    endif()

    if(CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE AND NOT ARGUMENTS_BUILD_TYPE)
        set(CONAN_CMAKE_MULTI ON)
        if (NOT ARGUMENTS_CONFIGURATION_TYPES)
            set(ARGUMENTS_CONFIGURATION_TYPES "Release;Debug")
        endif()
        message(STATUS "Conan: Using cmake-find-package-multi generator")
    else()
        set(CONAN_CMAKE_MULTI OFF)
    endif()

    conan_cmake_setup_conanfile(${ARGV})
    if(CONAN_CMAKE_MULTI)
        foreach(CMAKE_BUILD_TYPE ${ARGUMENTS_CONFIGURATION_TYPES})
            conan_cmake_settings(settings ${ARGV})
            conan_cmake_install(SETTINGS ${settings} ${ARGV})
        endforeach()
        set(CMAKE_BUILD_TYPE)
    else()
        conan_cmake_settings(settings ${ARGV})
        conan_cmake_install(SETTINGS ${settings} ${ARGV})
    endif()
endmacro()

macro(conan_check)
    # Checks conan availability in PATH
    # Arguments REQUIRED, DETECT_QUIET and VERSION are optional
    # Example usage:
    #    conan_check(VERSION 1.0.0 REQUIRED)
    set(options REQUIRED DETECT_QUIET)
    set(oneValueArgs VERSION)
    cmake_parse_arguments(CONAN "${options}" "${oneValueArgs}" "" ${ARGN})
    if(NOT CONAN_DETECT_QUIET)
        message(STATUS "Conan: checking conan executable")
    endif()

    find_program(CONAN_CMD conan)
    if(NOT CONAN_CMD AND CONAN_REQUIRED)
        message(FATAL_ERROR "Conan executable not found! Please install conan.")
    endif()
    if(NOT CONAN_DETECT_QUIET)
        message(STATUS "Conan: Found program ${CONAN_CMD}")
    endif()
    execute_process(COMMAND ${CONAN_CMD} --version
                    OUTPUT_VARIABLE CONAN_VERSION_OUTPUT
                    ERROR_VARIABLE CONAN_VERSION_OUTPUT)
    if(NOT CONAN_DETECT_QUIET)
        message(STATUS "Conan: Version found ${CONAN_VERSION_OUTPUT}")
    endif()

    if(DEFINED CONAN_VERSION)
        string(REGEX MATCH ".*Conan version ([0-9]+\\.[0-9]+\\.[0-9]+)" FOO
            "${CONAN_VERSION_OUTPUT}")
        if(${CMAKE_MATCH_1} VERSION_LESS ${CONAN_VERSION})
            message(FATAL_ERROR "Conan outdated. Installed: ${CMAKE_MATCH_1}, \
                required: ${CONAN_VERSION}. Consider updating via 'pip \
                install conan==${CONAN_VERSION}'.")
        endif()
    endif()
endmacro()

macro(conan_config_install)
    set(oneValueArgs ITEM)
    set(multiValueArgs ARGS)
    cmake_parse_arguments(CONAN "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    find_program(CONAN_CMD conan)
    if(NOT CONAN_CMD AND CONAN_REQUIRED)
        message(FATAL_ERROR "Conan executable not found!")
    endif()

    message(STATUS "Conan: Installing config from ${CONAN_ITEM}")
    execute_process(COMMAND ${CONAN_CMD} config install ${CONAN_ITEM})
endmacro()

conan_config_install(ITEM ${CMAKE_SOURCE_DIR}/conan/config)
set(CONAN_FILE conanfile.txt)
if (WIN32)
    set(CONAN_PROFILE windows-vs2017-native64)
else()
    set(CONAN_PROFILE linux-gcc7-native64)
endif()

if (CMAKE_BUILD_TYPE)
    conan_cmake_run(CONANFILE ${CONAN_FILE} PROFILE ${CONAN_PROFILE}-${CMAKE_BUILD_TYPE})
else()
    conan_cmake_run(CONANFILE ${CONAN_FILE} RELEASE_PROFILE ${CONAN_PROFILE}-Release)
endif()
