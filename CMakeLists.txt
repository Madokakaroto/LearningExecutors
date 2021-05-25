project(Executors)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fconcepts-ts")

add_executable(Executors Concepts.hpp Executors.hpp main.cpp)
link_libraries(Executors -lc++)