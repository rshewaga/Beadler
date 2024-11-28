cmake_minimum_required(VERSION 3.15)


#######################################################################################
# Configure project

set(PROJ_NAME Beadler)
project(
    ${PROJ_NAME}
    VERSION 1.0
    DESCRIPTION "Simple converter for TubiTV downloaded .srt subtitle files to proper .srt formatting."
    HOMEPAGE_URL https://www.robshewaga.com
    LANGUAGES CXX
)

# Release by default for consumers
set(CMAKE_BUILD_TYPE Debug)

# Semicolon-separated list of directories specifying a search path for CMake modules 
# to be loaded by include() or find_package()
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

#######################################################################################
# Setup Conan and find its packages

include(cmake/conan.cmake)
find_package(lyra REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(wxWidgets REQUIRED)
find_package(libgit2 REQUIRED)
find_package(EnTT REQUIRED)

#######################################################################################
# Other files added to a target for visibility

add_custom_target(
    ${PROJ_NAME}_other_files
    SOURCES
    README.md
    conanfile.py
    cmake/CompilerWarnings.cmake
    .gitignore
)

include(GNUInstallDirs)


#######################################################################################
# Link the following interface to set the c++ standard / compile-time options requested

add_library(${PROJ_NAME}_project_options INTERFACE)

target_compile_features(
    ${PROJ_NAME}_project_options
    INTERFACE
    cxx_std_20
)

target_compile_definitions(
    ${PROJ_NAME}_project_options
    INTERFACE
    CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}"
    CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    CMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
    PROJ_NAME="${PROJ_NAME}"
)

if(WIN32)
# For NOMINMAX on Windows, see https://stackoverflow.com/questions/2561368/illegal-token-on-right-side-of
target_compile_definitions(
    ${PROJ_NAME}_project_options
    INTERFACE
    NOMINMAX
)
endif()


#######################################################################################
# Link the following interface to include a standard set of libs that would be needed

add_library(${PROJ_NAME}_project_libraries INTERFACE)

target_link_libraries(
    ${PROJ_NAME}_project_libraries
    INTERFACE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    wxWidgets::wxWidgets
    libgit2::libgit2
    EnTT::EnTT
)


#######################################################################################
# Link the following interface to use the warnings specified in CompilerWarnings.cmake
add_library(
    ${PROJ_NAME}_project_warnings
    INTERFACE
)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJ_NAME}_project_warnings)


#######################################################################################

add_subdirectory(src)