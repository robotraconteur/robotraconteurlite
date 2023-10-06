cmake_minimum_required(VERSION 3.3)
project(robotraconteurlite)

if(CMAKE_COMPILER_IS_GNUCC)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Werror -Werror=return-type")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_compile_definitions(-DROBOTRACONTEURLITE_USE_OPENSSL=1)

add_library(robotraconteurlite 
							   include/robotraconteurlite/array.h
                               include/robotraconteurlite/config.h
							   include/robotraconteurlite/err.h
							   include/robotraconteurlite/message.h
							   include/robotraconteurlite/node.h
							   include/robotraconteurlite/nodeid.h
							   include/robotraconteurlite/robotraconteurlite.h
							   include/robotraconteurlite/tcp_transport.h
							   src/array.c
							   src/connection.c
							   src/message.c
							   src/node.c
							   src/nodeid.c
							   src/tcp_transport_posix.c
							   src/tcp_transport.c)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include(CTest)
if (BUILD_TESTING)
find_package(cmocka REQUIRED)
include_directories(${CMOCKA_INCLUDE_DIR})
add_executable(robotraconteurlite_array_test test/array_test.c)
target_link_libraries(robotraconteurlite_array_test robotraconteurlite ${CMOCKA_LIBRARY})
add_test(array_test robotraconteurlite_array_test)
add_executable(robotraconteurlite_message_test test/message_test.c)
target_link_libraries(robotraconteurlite_message_test robotraconteurlite ${CMOCKA_LIBRARY})
add_test(message_test robotraconteurlite_message_test)
endif()