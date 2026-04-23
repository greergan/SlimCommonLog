cmake_minimum_required(VERSION 3.20)

cmake_path(GET CMAKE_SOURCE_DIR FILENAME SLIM_LIB)
string(TOLOWER "${SLIM_LIB}" SLIM_LIB_LOWER)

if(DEFINED CPACK_OUTPUT_FILE_PREFIX)
    if("${CPACK_OUTPUT_FILE_PREFIX}" STREQUAL ".")
	    set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_SOURCE_DIR})
    endif()
else()
	message(FATAL_ERROR "CPACK_OUTPUT_FILE_PREFIX is not set")
endif()

include(FetchContent)

# ------------------------------------------------------------
# Local override controls (added only)
# ------------------------------------------------------------
set(SLIM_USE_LOCAL_SOURCE OFF CACHE BOOL "Use local library source instead of git")

if(SLIM_USE_LOCAL_SOURCE)
    set(GIT_TAG 0.0.0)
    set(SLIMVALUE_GIT_HASH "none")
else()
    execute_process(
        COMMAND git ls-remote --tags --sort=-v:refname https://github.com/greergan/${SLIM_LIB}.git
        OUTPUT_VARIABLE TAGS_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    string(REGEX MATCH "refs/tags/([^\\^\\n]+)" _ "${TAGS_RAW}")
    set(GIT_TAG ${CMAKE_MATCH_1})
endif()

# ------------------------------------------------------------
# FetchContent guarded (only change: conditional)
# ------------------------------------------------------------
if(SLIM_USE_LOCAL_SOURCE)
    set(slimlibrarysrc_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    message(STATUS "using LOCAL source (${slimlibrarysrc_SOURCE_DIR})")
else()
    FetchContent_Declare(
        SlimLibrarySrc
        GIT_REPOSITORY https://github.com/greergan/${SLIM_LIB}.git
        GIT_TAG        ${GIT_TAG}
    )
    message(STATUS "using GIT source (tag: ${GIT_TAG})")
    message(STATUS "fetched from https://github.com/greergan/${SLIM_LIB}.git")
    FetchContent_Populate(SlimLibrarySrc)
        execute_process(
        COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE SLIMVALUE_GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()

project(${SLIM_LIB} VERSION ${GIT_TAG} LANGUAGES CXX)

set(LIB_PC_IN_FILE ${SLIM_LIB_LOWER}.pc.in)
set(LIB_PC_FILE ${SLIM_LIB_LOWER}.pc)
set(HPP_IN_FILE "${slimlibrarysrc_SOURCE_DIR}/include/slim/${SLIM_LIB}.hpp.in")


if(NOT EXISTS ${LIB_PC_IN_FILE})
    message(FATAL_ERROR "${LIB_PC_IN_FILE} File not found")
endif()

if(EXISTS ${HPP_IN_FILE})
    set(HPP_HEADER_ONLY ON CACHE BOOL "Is .hpp header only library")
    set(HPP_OUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/slim/${SLIM_LIB}.hpp")
    message(STATUS "${HPP_IN_FILE} exists")
    set(SLIM_LIB_DESCRIPTION "${SLIM_LIB} header-only C++ library")
else()

endif()

set(SLIMVALUE_VERSION ${GIT_TAG})
set(SLIMVALUE_VERSION_FULL "${SLIMVALUE_VERSION}+${SLIMVALUE_GIT_HASH}")
execute_process(
	COMMAND git config --get user.name
	OUTPUT_VARIABLE GIT_USER_NAME
	OUTPUT_STRIP_TRAILING_WHITESPACE
	ERROR_QUIET
)

if(NOT GIT_USER_NAME)
	set(GIT_USER_NAME "unknown")
endif()

execute_process(
	COMMAND git config --get user.email
	OUTPUT_VARIABLE GIT_USER_EMAIL
	OUTPUT_STRIP_TRAILING_WHITESPACE
	ERROR_QUIET
)

if(NOT GIT_USER_EMAIL)
	set(GIT_USER_EMAIL "unknown")
endif()

message(STATUS "OS: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Project dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "Build dir:   ${CMAKE_BINARY_DIR}")
message(STATUS "Dist dir: ${CPACK_OUTPUT_FILE_PREFIX}")
message(STATUS "Local INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "slim library name: ${SLIM_LIB}")
message(STATUS  "Library .pc.in name: ${LIB_PC_IN_FILE}")

if(HPP_HEADER_ONLY)
    message(STATUS "install .hpp only: ${HPP_HEADER_ONLY}")
    message(STATUS ".hpp.in file: ${HPP_IN_FILE}")
    message(STATUS ".hpp file: ${HPP_OUT_FILE}")
else()
endif()

message(STATUS "version tag: ${GIT_TAG}")
message(STATUS "version hash: ${SLIMVALUE_GIT_HASH}")
message(STATUS "full version: ${SLIMVALUE_VERSION_FULL}")
message(STATUS "Package maintainer: ${GIT_USER_NAME} <${GIT_USER_EMAIL}>")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(HPP_HEADER_ONLY)
    configure_file(
        "${HPP_IN_FILE}"
        "${HPP_OUT_FILE}"
        @ONLY
    )
else()
endif()

add_library(slim INTERFACE)

target_include_directories(slim
    INTERFACE
        $<BUILD_INTERFACE:${slimlibrarysrc_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(slim INTERFACE cxx_std_23)
set(SLIM_CXX_STANDARD 23)

option(SLIM_BUILD_TESTS "Build tests" ON)

if(SLIM_BUILD_TESTS)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.0
    )

    FetchContent_MakeAvailable(Catch2)

    enable_testing()

    add_executable(slim_tests
        ${slimlibrarysrc_SOURCE_DIR}/tests/main.cpp
    )

    target_link_libraries(slim_tests
        PRIVATE slim Catch2::Catch2WithMain
    )

    include(CTest)
    include(Catch)
    catch_discover_tests(slim_tests)
endif()

install(TARGETS slim EXPORT SlimTargets)

if(HPP_HEADER_ONLY)
    install(FILES
        "${HPP_OUT_FILE}"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/slim
    )
else()
endif()

install(EXPORT SlimTargets
        FILE SlimTargets.cmake
        NAMESPACE Slim::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${SLIM_LIB})

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${SLIM_LIB}ConfigVersion.cmake"
    VERSION ${GIT_TAG}
    COMPATIBILITY AnyNewerVersion
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${SLIM_LIB}Config.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/SlimTargets.cmake\")\n")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${SLIM_LIB}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${SLIM_LIB}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${SLIM_LIB}
)

configure_file(
    "${slimlibrarysrc_SOURCE_DIR}/${LIB_PC_IN_FILE}"
    "${CMAKE_CURRENT_BINARY_DIR}/${LIB_PC_FILE}"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${LIB_PC_FILE}"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH)
if(ARCH MATCHES "x86_64|amd64")
    set(ARCH_NAME "amd64")
elseif(ARCH MATCHES "i386|i686")
    set(ARCH_NAME "x86")
elseif(ARCH MATCHES "aarch64|arm64")
    set(ARCH_NAME "arm64")
elseif(ARCH MATCHES "armv7|arm")
    set(ARCH_NAME "arm")
else()
    set(ARCH_NAME "unknown")
endif()

set(CPACK_GENERATOR "DEB;RPM")

set(CPACK_PACKAGE_NAME ${SLIM_LIB_LOWER})
set(CPACK_PACKAGE_VERSION ${GIT_TAG})
set(CPACK_PACKAGE_CONTACT "${GIT_USER_NAME} <${GIT_USER_EMAIL}>")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${ARCH_NAME}")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${GIT_USER_NAME})
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${ARCH_NAME})
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

set(CPACK_RPM_PACKAGE_NAME ${SLIM_LIB_LOWER})
set(CPACK_RPM_PACKAGE_VERSION ${GIT_TAG})
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
set(CPACK_RPM_PACKAGE_SUMMARY ${SLIM_LIB_DESCRIPTION})
set(CPACK_RPM_PACKAGE_ARCHITECTURE ${ARCH_NAME})
set(CPACK_RPM_PACKAGE_PREFIX "${CMAKE_INSTALL_PREFIX}")

include(CPack)