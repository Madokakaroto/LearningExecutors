#pragma once

namespace std::execution
{
    class static_thread_pool
    {
        struct thread_state
        {

        };

    public:
        explicit static_thread_pool(size_t num_threads)
            : num_threads_(num_threads)
        {

        }

    private:
        size_t const num_threads_;
    };
}