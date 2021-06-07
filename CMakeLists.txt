cmake_minimum_required(VERSION 3.17)

project(Executors)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fconcepts-ts")

set(EXECUTORS_CORE
        Core/Forward.hpp
        Core/Traits.hpp
        Core/Adaptor/AsInvocable.hpp
        Core/Adaptor/AsOperation.hpp
        Core/Adaptor/AsReceiver.hpp
        Core/Algorithm/Connect.hpp
        Core/Algorithm/Execute.hpp
        Core/Algorithm/SetDone.hpp
        Core/Algorithm/SetError.hpp
        Core/Algorithm/SetValue.hpp
        Core/Algorithm/Start.hpp
        Core/Algorithm/Submit.hpp
        Core/Concepts/Basic.hpp
        Core/Concepts/Executor.hpp
        Core/Concepts/OperationState.hpp
        Core/Concepts/Receiver.hpp
        Core/Concepts/Sender.hpp
        Core/Concepts/Std.hpp
    )

add_executable(Executors
        ${EXECUTORS_CORE}
        Core.hpp
        Executors.hpp
        Forward.hpp
        main.cpp)

link_libraries(Executors -lc++)