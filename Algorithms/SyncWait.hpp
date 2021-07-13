#pragma once

namespace std::execution
{
    namespace sync_wait_n
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

        template <typename ValueTypes, typename ErrorTypes>
        class _receiver_type
        {
        public:
            using promise_t = _promise<ValueTypes, ErrorTypes>;
            using value_types = typename promise_t::value_types;
            using error_types = typename promise_t::error_types;

        public:
            explicit _receiver_type(promise_t& promise, _sync_primitive& sync)
                : promise_(promise)
                , sync_(sync)
            {}

            template <typename ... Args>
            void set_value(Args&& ... args) && noexcept(promise_t::template is_nothrow_values<Args&&...>())
            {
                promise_.set_value(forward<Args>(args)...);
                sync_.notify();
            }

            void set_error(error_types&& e) && noexcept
            {
                promise_.set_error(move(e));
                sync_.notify();
            }

            void set_done() && noexcept
            {
                promise_.set_done();
                sync_.notify();
            }

        private:
            promise_t&          promise_;
            _sync_primitive&    sync_;
        };

        template <typename S>
        concept has_sync_wait_impl =
            requires(S&& s)
            {
                forward<S>(s).sync_wait();
            };

        template <typename S>
        concept customise_point =
            requires(S&& s)
            {
                sync_wait(forward<S>(s));
            };

        struct func_type
        {
            // default impl
            template <typename S> requires(sender<S>)
            auto operator() (S&& s) const
            {
                // get the type of recevier
                using receiver_type = _receiver_type
                <
                    typename sender_traits<remove_cvref_t<S>>::template value_types<variant, tuple>,
                    typename sender_traits<remove_cvref_t<S>>::template error_types<variant>
                >;

                // get the type of promise
                using promise_t = typename receiver_type::promise_t;

                // construct a promise
                promise_t promise{};

                // construct a synchronise primitive object
                _sync_primitive sync;

                // construct a receiver
                receiver_type receiver{ promise, sync };

                // start the async operation
                execution::start(execution::connect(forward<S>(s), move(receiver)));

                // wait on this thread
                sync.wait();

                // return value
                return promise.get_value();
            }

            // customise point implementation
            template <typename S> requires(sender<S> && customise_point<S>)
            auto operator() (S&& s) const
            {
                return sync_wait(forward<S>(s));
            }

            // has sync_wait implementation
            template <typename S> requires(sender<S> && has_sync_wait_impl<S> && !customise_point<S>)
            auto operator() (S&& s) const
            {
                return forward<S>(s).sync_wait();
            }
        };
    }

    inline constexpr sync_wait_n::func_type sync_wait{};
}