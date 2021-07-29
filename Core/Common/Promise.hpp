#pragma once

namespace std::execution
{
    class _sync_primitive
    {
    public:
        void notify() noexcept
        {
            unique_lock<mutex> lock{ mutex_ };
            stop_ = true;
            cond_var_.notify_one();
        }

        void wait()
        {
            unique_lock<mutex> lock{ mutex_ };
            cond_var_.wait(lock, [this]{ return stop_; });
        }

    private:
        bool                stop_{ false };
        condition_variable  cond_var_;
        mutex               mutex_;
    };

    struct _promise_base
    {
        enum class state_t { incomplete, done, value, error, };
        state_t state_{ state_t::incomplete };

        template <typename ValueTypes, typename ErrorTypes>
        void check_state(variant<ValueTypes, ErrorTypes> const& v) const noexcept
        {
            switch(state_)
            {
                case state_t::value:
                    break;
                case state_t::error:
                {
                    if constexpr(same_as<ErrorTypes, exception_ptr>)
                    {
                        rethrow_exception(get<1>(v.value_));
                    }
                    else
                    {
                        throw get<1>(v.value_);
                    }
                }
                default:
                    terminate();
            }
        }
    };

    template <typename Values, typename Errors>
    struct _promise : _promise_base
    {
        // export value types and error types
        using value_types = get_value_types_t<Values>;
        using error_types = get_error_types_t<Errors>;
        using value_type = variant<value_types, error_types>;

        template <typename ... Args>
        static consteval bool is_nothrow_values() noexcept
        {
            return conjunction_v<
                is_nothrow_move_constructible<value_types>,
                is_nothrow_move_assignable<value_types>,
                is_nothrow_constructible<value_types, Args...>>;
        }

        static consteval bool is_nothrow_errors() noexcept
        {
            return conjunction_v<is_nothrow_move_constructible<error_types>, is_nothrow_move_assignable<error_types>>;
        }

        //static_assert(is_nothrow_errors());

        value_types get_value() const
        {
            return get<0>(value_);
        }

        template <typename ... Args>
        void set_value(Args&& ... args) noexcept
        {
            if constexpr(is_nothrow_values<Args&&...>())
            {
                value_ = value_types{ forward<Args>(args)... };
                this->state_ = state_t::value;
            }
            else
            {
                try
                {
                    value_ = value_types{ forward<Args>(args)... };
                    this->state_ = state_t::value;
                }
                catch(...)
                {
                    set_error(current_exception());
                }
            }
        }

        void set_error(error_types&& e) noexcept
        {
            value_ = move(e);
            this->state_ = state_t::error;
        }

        void set_done() noexcept
        {
            this->state_ = state_t::done;
        }

        value_type value_;
    };

    template <typename Errors>
    struct _promise<void, Errors> : _promise_base
    {
        using value_types = void;
        using error_types = get_error_types_t<Errors>;
        using value_type = variant<int, error_types>;

        template <typename ... Args>
        static consteval bool is_nothrow_values() noexcept { return true; }

        static consteval bool is_nothrow_errors() noexcept
        {
            return conjunction_v<is_nothrow_move_constructible<error_types>, is_nothrow_move_assignable<error_types>>;
        }

        //static_assert(is_nothrow_errors());

        void get_value() const
        {
            [[maybe_unused]]int value = get<0>(value_);
            return;
        }

        void set_value(...) noexcept
        {
            value_ = 0;
            this->state_ = state_t::value;
        }

        void set_error(error_types&& e) noexcept
        {
            value_ = move(e);
            this->state_ = state_t::error;
        }

        void set_done() noexcept
        {
            this->state_ = state_t::done;
        }

        value_type value_;
    };
}