set(PYTHON_EXECUTABLE python3 CACHE STRING "Python executable")

add_test(NAME tiny_test_client COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tiny_client_ci_test.py
                                       --build-dir ${CMAKE_BINARY_DIR}/examples WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_test(NAME tiny_test_service COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tiny_service_ci_test.py
                                        --build-dir ${CMAKE_BINARY_DIR}/examples WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
