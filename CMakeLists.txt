cmake_minimum_required(VERSION 3.3)
project(robotraconteurlite VERSION 0.1.0)

# if(CMAKE_COMPILER_IS_GNUCC)
set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -ansi -pedantic -Werror -Werror=return-type -Wimplicit-function-declaration -Wint-conversion -Wincompatible-pointer-types"
)
# endif()

find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(uuid REQUIRED IMPORTED_TARGET uuid)

add_definitions(-DROBOTRACONTEURLITE_USE_OPENSSL=1)

add_library(
    robotraconteurlite
    include/robotraconteurlite/array.h
    include/robotraconteurlite/config.h
    include/robotraconteurlite/err.h
    include/robotraconteurlite/message.h
    include/robotraconteurlite/node.h
    include/robotraconteurlite/nodeid.h
    include/robotraconteurlite/robotraconteurlite.h
    include/robotraconteurlite/tcp_transport.h
    include/robotraconteurlite/clock.h
    src/array.c
    src/connection.c
    src/message.c
    src/node.c
    src/nodeid.c
    src/tcp_transport_posix.c
    src/tcp_transport.c
    src/clock_posix.c
    src/poll.c
    src/poll_posix.c)

target_link_libraries(robotraconteurlite PUBLIC OpenSSL::Crypto uuid)

target_include_directories(robotraconteurlite PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
                                                     $<INSTALL_INTERFACE:include/>)

include(GNUInstallDirs)

install(
    TARGETS robotraconteurlite
    EXPORT robotraconteurliteTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
    PATTERN "*.h*")

export(TARGETS robotraconteurlite FILE "${CMAKE_CURRENT_BINARY_DIR}/robotraconteurliteTargets.cmake")
configure_file(cmake/robotraconteurliteConfig.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/robotraconteurliteConfig.cmake"
               @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/robotraconteurliteConfig.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
install(EXPORT robotraconteurliteTargets FILE robotraconteurliteTargets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

include(CTest)
if(BUILD_TESTING)
    find_package(cmocka REQUIRED)
    include_directories(${CMOCKA_INCLUDE_DIR})
    add_executable(robotraconteurlite_array_test test/array_test.c)
    target_link_libraries(robotraconteurlite_array_test robotraconteurlite ${CMOCKA_LIBRARY})
    add_test(array_test robotraconteurlite_array_test)
    add_executable(robotraconteurlite_message_test test/message_test.c)
    target_link_libraries(robotraconteurlite_message_test robotraconteurlite ${CMOCKA_LIBRARY})
    add_test(message_test robotraconteurlite_message_test)
endif()

option(BUILD_EXAMPLES "Build examples" ON)

if(BUILD_EXAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif()

if(RUN_TINY_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test/tiny_test)
endif()
