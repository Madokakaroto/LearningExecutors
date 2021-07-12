#pragma once

namespace std::execution
{
    class static_thread_pool;

    namespace static_thread_pool_n
    {
        template <receiver_of<> R>
        struct _op;

        struct _sender_type
        {
            // begin of sender traits
            template
            <
                template <typename ...> class Variant,
                template <typename ...> class Tuple
            >
            using value_types = Variant<Tuple<>>;

            template
            <
                template <typename ...> class Variant
            >
            using error_types = Variant<>;

            static constexpr bool sends_done = true;
            // end of sender traits

            // the context
            execution::static_thread_pool& pool_;

            // connect
            template <receiver_of<> Receiver>
            _op<remove_cvref_t<Receiver>> connect(Receiver&& r) const;

            // submit
            template <receiver_of<> Receiver>
            void submit(Receiver&& r) const;
        };

        struct _scheduler_type
        {
            friend execution::static_thread_pool;

            // concstructor
            _scheduler_type(execution::static_thread_pool& pool);

            // scheduler interface
            _sender_type schedule() const noexcept;

            // operator (not) equal compare
            friend bool operator ==(_scheduler_type const& lhs, _scheduler_type const& rhs) noexcept;
            friend bool operator !=(_scheduler_type const& lhs, _scheduler_type const& rhs) noexcept;

            execution::static_thread_pool& pool_;
        };
    }

    class static_thread_pool
    {
        template <receiver_of<> R>
        friend struct static_thread_pool_n::_op;
        friend static_thread_pool_n::_scheduler_type;

    public:
        using scheduler = static_thread_pool_n::_scheduler_type;
        using sender = static_thread_pool_n::_sender_type;

    public:
        static_thread_pool();
        explicit static_thread_pool(size_t num_threads);
        ~static_thread_pool();

        void stop();
        void join();
        void enqueue(task_t&& task);
        scheduler get_scheduler() noexcept;

    private:
        void init();
        void worker();

    private:
        size_t const    num_threads_;
        atomic<bool>    running_;
        vector<thread>  threads_;
        blocking_queue  queue_;
    };

    namespace static_thread_pool_n
    {
        // definition of _op
        template <receiver_of<> R>
        struct _op
        {
            execution::static_thread_pool& pool_;
            R r_;

            void start() &&
            {
                pool_.enqueue([r = move(r_)]() mutable noexcept
                {
                    try
                    {
                        execution::set_value(move(r));
                    }
                    catch (...)
                    {
                        execution::set_error(move(r), current_exception());
                    }
                });
            }
        };

        // implementation of _sender_type
        template <receiver_of<> Receiver>
        inline _op<remove_cvref_t<Receiver>> _sender_type::connect(Receiver&& r) const
        {
            return _op<remove_cvref_t<Receiver>>{ pool_, move(r) };
        }

        template <receiver_of<> Receiver>
        inline void _sender_type::submit(Receiver&& r) const
        {
            execution::start(execution::connect(move(*this), forward<Receiver>(r)));
        }

        // implementation of _scheduler_type
        inline _scheduler_type::_scheduler_type(execution::static_thread_pool& pool)
            : pool_(pool)
        {}

        inline _sender_type _scheduler_type::schedule() const noexcept
        {
            return _sender_type{ this->pool_ };
        }

        inline bool operator ==(_scheduler_type const& lhs, _scheduler_type const& rhs) noexcept
        {
            return addressof(lhs.pool_) == addressof(rhs.pool_);
        }

        inline bool operator !=(_scheduler_type const& lhs, _scheduler_type const& rhs) noexcept
        {
            return !(lhs == rhs);
        }
    }

    // implementation of static_thread_pool
    inline static_thread_pool::static_thread_pool()
        : static_thread_pool(thread::hardware_concurrency())
    {
    }

    inline static_thread_pool::static_thread_pool(size_t num_threads)
        : num_threads_(num_threads)
        , running_(true)
    {
        init();
    }

    inline static_thread_pool::~static_thread_pool()
    {
        stop();
        join();
    }

    inline void static_thread_pool::stop()
    {
        running_.store(false);
    }

    inline void static_thread_pool::join()
    {
        for(auto& thread : threads_)
        {
            if(thread.joinable())
            {
                thread.join();
            }
        }
    }

    inline void static_thread_pool::enqueue(task_t&& task)
    {
        queue_.enqueue(move(task));
    }

    inline static_thread_pool::scheduler static_thread_pool::get_scheduler() noexcept
    {
        return scheduler{ *this };
    }

    inline void static_thread_pool::init()
    {
        threads_.reserve(num_threads_);
        for(size_t loop = 0; loop < num_threads_; ++loop)
        {
            threads_.emplace_back([this]{ worker(); });
        }
    }

    inline void static_thread_pool::worker()
    {
        while(running_.load())
        {
            task_t task = queue_.dequeue([this]{ return running_.load(); });
            if(task)
            {
                task();
            }
        }
    }
}