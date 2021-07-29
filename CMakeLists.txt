cmake_minimum_required(VERSION 3.17)

project(Executors)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fconcepts-ts")

set(EXECUTORS_CORE
        Core/Core.hpp
        Core/Forward.hpp
        Core/Properties.hpp
        Core/Common/DoneReceiver.hpp
        Core/Common/ErrorReceiver.hpp
        Core/Common/Promise.hpp
        Core/Common/ValueReceiver.hpp
        Core/Common/Propagate.hpp
        Core/Concepts/Basic.hpp
        Core/Concepts/Executor.hpp
        Core/Concepts/OperationState.hpp
        Core/Concepts/Properties.hpp
        Core/Concepts/Receiver.hpp
        Core/Concepts/Scheduler.hpp
        Core/Concepts/Sender.hpp
        Core/Concepts/SenderTo.hpp
        Core/Concepts/Std.hpp
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
        Core/Traits/CallableTraits.hpp
        Core/Traits/Traits.hpp
        Core/Traits/TraitsImpl.hpp
    )

set(EXECUTORS_ALGORITHMS
        Algorithms/Algorithms.hpp
        Algorithms/EnsureStarted.hpp
        Algorithms/Just.hpp
        Algorithms/JustOn.hpp
        Algorithms/LetError.hpp
        Algorithms/LetValue.hpp
        Algorithms/On.hpp
        Algorithms/PipeOperator.hpp
        Algorithms/SyncWait.hpp
        Algorithms/Transform.hpp
        Algorithms/WhenAll.hpp
        )

set(EXECUTORS_MEMORY
        Memory/BlockingQueue.hpp
        Memory/Memory.hpp
    )

set(EXECUTORS_CONTEXTS
        Contexts/StaticThreadPool.hpp
        Contexts/ImmediateContext.hpp
    )

add_executable(Executors
        ${EXECUTORS_CORE}
        ${EXECUTORS_ALGORITHMS}
        ${EXECUTORS_MEMORY}
        Executors.hpp
        Test/main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

message(${CMAKE_CXX_COMPILER_ID})
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_definitions(USE_STD_CONCEPTS_LIB)
endif()

link_libraries(Executors -lc++ -lstdc++)