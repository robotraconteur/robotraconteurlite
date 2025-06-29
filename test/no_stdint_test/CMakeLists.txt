# add ${CMAKE_SOURCE_DIR} prefix to elements of ROBOTRACONTEURLITE_SRC

set(NOSTDINT_ROBOTRACONTEURLITE_SRC "")
foreach(file ${ROBOTRACONTEURLITE_SRC})
    list(APPEND NOSTDINT_ROBOTRACONTEURLITE_SRC "${CMAKE_SOURCE_DIR}/${file}")
endforeach()

add_executable(
    robotraconteurlite_no_stdint_test ${NOSTDINT_ROBOTRACONTEURLITE_SRC}
                                      ${CMAKE_SOURCE_DIR}/examples/tiny_service/robotraconteurlite_tiny_service.c)
target_include_directories(robotraconteurlite_no_stdint_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}
                                                                     ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(robotraconteurlite_no_stdint_test PRIVATE ROBOTRACONTEURLITE_NO_STDINT
                                                                     ROBOTRACONTEURLITE_USE_LOCAL_CONFIG)

# copy dependencies from robotraconteurlite library
if(ROBOTRACONTEURLITE_USE_OPENSSL)
    target_link_libraries(robotraconteurlite_no_stdint_test PUBLIC OpenSSL::Crypto)
endif()

if(ROBOTRACONTEURLITE_USE_LIBUUID AND NOT APPLE)
    target_link_libraries(robotraconteurlite_no_stdint_test PUBLIC uuid)
endif()

if(WIN32)
    target_link_libraries(robotraconteurlite_no_stdint_test PUBLIC ws2_32 Rpcrt4 Crypt32)
endif()
