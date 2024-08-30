add_test(NAME tiny_test_client COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/tiny_client_ci_test.py --build-dir
                                       ${CMAKE_BINARY_DIR}/examples WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_test(NAME tiny_test_service COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/tiny_service_ci_test.py --build-dir
                                        ${CMAKE_BINARY_DIR}/examples WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
