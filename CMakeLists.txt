project(Executors)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++2a -stdlib=libc++")

add_executable(Executors Executors.hpp main.cpp)