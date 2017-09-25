set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")

set(TEST_SOURCES
    full_test.cpp
    )

add_executable(test_executable ${TEST_SOURCES})
