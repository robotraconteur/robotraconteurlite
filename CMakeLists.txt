cmake_minimum_required(VERSION 3.5)
project(robotraconteurlite VERSION 0.1.0)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} -ansi -pedantic -Werror -Werror=return-type -Wimplicit-function-declaration -Wint-conversion -Wincompatible-pointer-types -Wall -Wno-unused-function"
    )
endif()

if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(uuid REQUIRED IMPORTED_TARGET uuid)
    option(ROBOTRACONTEURLITE_USE_OPENSSL "Use OpenSSL library" TRUE)
    mark_as_advanced(ROBOTRACONTEURLITE_USE_OPENSSL)
    option(ROBOTRACONTEURLITE_POSIX "Use POSIX API" TRUE)
    mark_as_advanced(ROBOTRACONTEURLITE_POSIX)
    option(ROBOTRACONTEURLITE_USE_LIBUUID "Use libuuid library" TRUE)
    mark_as_advanced(ROBOTRACONTEURLITE_USE_LIBUUID)
endif()

if(WIN32)
    option(ROBOTRACONTEURLITE_USE_OPENSSL "Use OpenSSL library" FALSE)
    mark_as_advanced(ROBOTRACONTEURLITE_USE_OPENSSL)
endif()

if(ROBOTRACONTEURLITE_USE_OPENSSL)
    find_package(OpenSSL REQUIRED)
    add_definitions(-DROBOTRACONTEURLITE_USE_OPENSSL=1)
endif()

set(ROBOTRACONTEURLITE_SRC
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
    src/array_types.c
    src/connection.c
    src/message.c
    src/message_data.c
    src/node.c
    src/nodeid.c
    src/tcp_transport.c
    src/poll.c)

if(ROBOTRACONTEURLITE_POSIX)
    set(ROBOTRACONTEURLITE_SRC ${ROBOTRACONTEURLITE_SRC} src/tcp_transport_posix.c src/clock_posix.c src/poll_posix.c)
endif()

if(ROBOTRACONTEURLITE_USE_LIBUUID)
    set(ROBOTRACONTEURLITE_SRC ${ROBOTRACONTEURLITE_SRC} src/nodeid_libuuid.c)
endif()

if(WIN32)
    set(ROBOTRACONTEURLITE_SRC ${ROBOTRACONTEURLITE_SRC} src/tcp_transport_win32.c src/clock_win32.c src/poll_win32.c
                               src/nodeid_win32.c)
endif()

add_library(robotraconteurlite ${ROBOTRACONTEURLITE_SRC})

if(ROBOTRACONTEURLITE_USE_OPENSSL)
    target_link_libraries(robotraconteurlite PUBLIC OpenSSL::Crypto)
endif()

if(ROBOTRACONTEURLITE_USE_LIBUUID AND NOT APPLE)
    target_link_libraries(robotraconteurlite PUBLIC uuid)
endif()

if(WIN32)
    target_link_libraries(robotraconteurlite PUBLIC ws2_32 Rpcrt4 Crypt32)
    include(GenerateExportHeader)
    generate_export_header(robotraconteurlite EXPORT_MACRO_NAME ROBOTRACONTEURLITE_API)
    target_include_directories(robotraconteurlite PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
    if(NOT BUILD_SHARED_LIBS)
        target_compile_definitions(robotraconteurlite PUBLIC ROBOTRACONTEURLITE_STATIC_DEFINE)
    endif()
endif()

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

if(WIN32)
    install(FILES $<TARGET_FILE_DIR:robotraconteurlite>/robotraconteurlite_export.h
            DESTINATION include/robotraconteurlite)
endif()

export(TARGETS robotraconteurlite FILE "${CMAKE_CURRENT_BINARY_DIR}/robotraconteurliteTargets.cmake")
configure_file(cmake/robotraconteurliteConfig.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/robotraconteurliteConfig.cmake"
               @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/robotraconteurliteConfig.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
install(EXPORT robotraconteurliteTargets FILE robotraconteurliteTargets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()

option(BUILD_EXAMPLES "Build examples" ON)

if(BUILD_EXAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif()

if(RUN_TINY_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test/tiny_test)
endif()
