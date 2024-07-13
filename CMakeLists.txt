cmake_minimum_required(VERSION 3.10)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

include_directories(include common/SimpleAmqpClient common/argparse/include)
configure_file(include/version.h.in version.h)

project(PerformanceTest VERSION ${PROJECT_VERSION})


add_library(lib include/profile.hpp src/main.cpp)
add_executable(PerformanceTest src/main.cpp)
target_include_directories(PerformanceTest PRIVATE include common/SimpleAmqpClient)
target_link_libraries(PerformanceTest lib SimpleAmqpClient)

set_target_properties(PerformanceTest lib PROPERTIES
                CXX_STANDARD 17
                CXX_STANDARD_REQUIRED ON)


