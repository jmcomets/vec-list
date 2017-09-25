set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3")

set(BENCH_SOURCES
    bench.cpp
    demangle.cpp
    graphs.cpp
    )

add_executable(bench_executable ${BENCH_SOURCES})
