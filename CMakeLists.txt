cmake_minimum_required(VERSION 3.17)

project(Executors)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fconcepts-ts")

set(EXECUTORS_CORE
        Core/Core.hpp
        Core/Forward.hpp
        Core/Properties.hpp
        Core/Traits.hpp
        Core/TraitsImpl.hpp
        Core/Adaptors/AsDoneReceiver.hpp
        Core/Adaptors/AsErrorReceiver.hpp
        Core/Adaptors/AsValueReceiver.hpp
        Core/CPOs/BulkExecute.hpp
        Core/CPOs/Connect.hpp
        Core/CPOs/Execute.hpp
        Core/CPOs/Properties.hpp
        Core/CPOs/Schedule.hpp
        Core/CPOs/SetDone.hpp
        Core/CPOs/SetError.hpp
        Core/CPOs/SetValue.hpp
        Core/CPOs/StandardExt.hpp
        Core/CPOs/Start.hpp
        Core/CPOs/Submit.hpp
        Core/Concepts/Basic.hpp
        Core/Concepts/Executor.hpp
        Core/Concepts/OperationState.hpp
        Core/Concepts/Properties.hpp
        Core/Concepts/Receiver.hpp
        Core/Concepts/Scheduler.hpp
        Core/Concepts/Sender.hpp
        Core/Concepts/SenderTo.hpp
        Core/Concepts/Std.hpp
    )

set(EXECUTORS_ALGORITHMS
        Algorithms/Algorithms.hpp
        Algorithms/Just.hpp
        Algorithms/On.hpp
        Algorithms/SyncWait.hpp
        Algorithms/Then.hpp
        )

set(EXECUTORS_MEMORY
        Memory/BlockingQueue.hpp
        Memory/Memory.hpp
    )

set(EXECUTORS_CONTEXTS
        Contexts/StaticThreadPool.hpp
    )

add_executable(Executors
        ${EXECUTORS_CORE}
        ${EXECUTORS_ALGORITHMS}
        ${EXECUTORS_MEMORY}
        Executors.hpp
        Test/main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(Executors -lc++ -lstdc++)