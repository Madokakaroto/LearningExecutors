#pragma once

namespace std::execution
{
    // Associated execution context property
    struct context_t
    {
        template <typename T>
        static constexpr bool is_applicable_property_v = executor<T>;

        static constexpr bool is_requirable = false;
        static constexpr bool is_perferable = false;

        template <typename Executor>
        static constexpr decltype(auto) static_query_v = Executor::query(std::declval<context_t>());
    };
    constexpr context_t context;

    // Blocking properties:
    // The `blocking_t` property describes what guarantees executors provide about the blocking behavior of their execution functions.
    struct blocking_t
    {
        // Invocation of an executor’s execution function may block pending completion
        // of one or more invocations of the submitted function object.
        struct possibly_t
        {
        };
        static constexpr possibly_t possibly{};

        // Invocation of an executor’s execution function shall block until completion
        // of all invocations of submitted function object.
        struct always_t
        {
        };
        static constexpr always_t always{};

        // Invocation of an executor’s execution function shall not block pending completion
        // of the invocations of the submitted function object.
        struct never_t
        {
        };
        static constexpr never_t never{};
    };
    constexpr blocking_t block{};

    // Properties to indicate if submitted tasks represent continuations
    // The `relationship_t` property allows users of executors to indicate that submitted tasks represent continuations.
    struct relationship_t
    {
        // Function objects submitted through the executor do not represent continuations of the caller.
        struct fork_t
        {
        };
        static constexpr fork_t fork{};

        // Function objects submitted through the executor represent continuations of the caller.
        // Invocation of the submitted function object may be deferred until the caller completes.
        struct continuation_t
        {
        };
        static constexpr continuation_t continuation{};
    };
    constexpr relationship_t relationship{};

    // Properties to indicate likely task submission in the future
    // The `outstanding_work_t` property allows users of executors to indicate that task submission is likely in the future.
    struct outstanding_work_t
    {
        // The existence of the executor object does not indicate any likely future submission of a function object.
        struct untracked_t
        {
        };
        static constexpr untracked_t untracked{};

        // The existence of the executor object represents an indication of likely future submission of a function object.
        // The executor or its associated execution context may choose to maintain execution resources in anticipation of this submission.
        struct tracked_t
        {
        };
        static constexpr tracked_t tracked{};
    };
    constexpr outstanding_work_t outstanding_work{};

    // Properties for bulk execution guarantees
    struct bulk_guarantee_t
    {
        // Execution agents within the same bulk execution may be parallelized and vectorized.
        struct unsequenced_t
        {
        };
        static constexpr unsequenced_t unsequenced{};

        // Execution agents within the same bulk execution may not be parallelized.
        struct sequenced_t
        {
        };
        static constexpr sequenced_t sequenced{};

        // Execution agents within the same bulk execution may be parallelized.
        struct parallel_t
        {
        };
        static constexpr parallel_t parallel{};
    };
    constexpr bulk_guarantee_t bulk_guarantee{};

    // Properties for mapping of execution on to threads
    // The mapping_t property describes what guarantees executors provide about the mapping of execution agents onto threads of execution.
    struct mapping_t
    {
        // Execution agents are mapped onto threads of execution.
        struct thread_t
        {
        };
        static constexpr thread_t thread{};

        // Each execution agent is mapped onto a new thread of execution.
        struct new_thread_t
        {
        };
        static constexpr new_thread_t new_thread{};

        // Mapping of each execution agent is implementation-defined.
        struct other_t
        {
        };
        static constexpr other_t other{};
    };
    constexpr mapping_t mapping{};
}