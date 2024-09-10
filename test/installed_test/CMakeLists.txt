cmake_minimum_required(VERSION 3.8)
project(robotraconteurlite_installed_test)

find_package(robotraconteurlite REQUIRED)

add_executable(robotraconteurlite_tiny_client
               ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/tiny_client/robotraconteurlite_tiny_client.c)
target_link_libraries(robotraconteurlite_tiny_client robotraconteurlite)
