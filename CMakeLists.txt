cmake_minimum_required(VERSION 4.0)

project(CoronaLogger
    VERSION   1.0.0
    LANGUAGES CXX
)

set(CORONA_LOGGER_VERSION ${PROJECT_VERSION})
set(CMAKE_C_STANDARD              11)
set(CMAKE_C_STANDARD_REQUIRED     ON)
set(CMAKE_C_EXTENSIONS            ON)
set(CMAKE_CXX_STANDARD            20)
set(CMAKE_CXX_STANDARD_REQUIRED   ON)
set(CMAKE_CXX_EXTENSIONS          ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CORONA_LOGGER_BUILD_EXAMPLES "Build CoronaLogger example executables." ${PROJECT_IS_TOP_LEVEL})

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.3
)

FetchContent_MakeAvailable(spdlog)

add_subdirectory(src)

if(CORONA_LOGGER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
