cmake_minimum_required(VERSION 3.10)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

include_directories(include common/SimpleAmqpClient common/argparse/include common/SimpleAmqpClient/src)
configure_file(include/version.h.in version.h)

project(PerformanceTest VERSION ${PROJECT_VERSION})

find_package(rabbitmq-c REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${Rabbitmqc_INCLUDE_DIRS})

add_library(lib include/profile.hpp src/main.cpp
        include/args.hpp
        include/SimpleAmqpExecutor.hpp
        src/SimpleAmqpExecutor.cpp
        include/RabbitmqcExecutor.hpp
        src/RabbitmqcExecutor.cpp)

add_executable(PerformanceTest src/main.cpp)
#find_package(Boost 1.47.0 COMPONENTS chrono system REQUIRED)
#include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
#link_directories(${Boost_LIBRARY_DIRS})

target_include_directories(PerformanceTest PRIVATE include common/SimpleAmqpClient)
target_link_libraries(PerformanceTest lib SimpleAmqpClient rabbitmq::rabbitmq)

set_target_properties(PerformanceTest lib PROPERTIES
                CXX_STANDARD 17
                CXX_STANDARD_REQUIRED ON)


