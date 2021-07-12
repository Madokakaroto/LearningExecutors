#pragma once

namespace std::execution
{
    namespace sync_wait_n
    {
        struct shared_state_base
        {
            condition_variable  cond_var_;
            mutex               mutex_;
        };

        enum class status_t
        {
            none,
            done,
        };

        template <typename T, typename E = exception_ptr>
        class shared_state : protected shared_state_base
        {
        private:
            variant<status_t, T, E> values_;

        public:
            static consteval size_t size() noexcept
            {
                using tuple_size_type = tuple_size<T>;
                if constexpr(is_complete_type_v<tuple_size_type>)
                {
                    tuple_size_type::value;
                }
                return size_t{ 1 };
            }

            shared_state() : values_(status_t::none) {}

            template <typename ... Args>
            void set_value(Args&& ... args)
            {
                unique_lock<mutex> lock{ mutex_ };
                values_ = move(T( forward<Args>(args)... ));
                cond_var_.notify_one();
            }

            void set_error(E e)
            {
                unique_lock<mutex> lock{ mutex_ };
                values_ = move(e);
                cond_var_.notify_one();
            }

            void set_done()
            {
                unique_lock<mutex> lock{ mutex_ };
                values_ = status_t::done;
                cond_var_.notify_one();
            }

            void wait()
            {
                unique_lock<mutex> lock{ mutex_ };
                cond_var_.wait(lock, [this]
                {
                    auto const index = values_.index();
                    return index != 0 || get<0>(values_) != status_t::none;
                });
            }

            T& values()
            {
                auto const index = values_.index();
                if(index == 1)
                {
                    return get<1>(values_);
                }
                else if(index == 2)
                {
                    if constexpr(same_as<E, exception_ptr>)
                    {
                        rethrow_exception(get<2>(values_));
                    }
                    else
                    {
                        throw get<2>(values_);
                    }
                }
                else
                {
                    if (get<0>(values_) == status_t::done)
                    {
                        terminate();
                    }
                    else
                    {
                        throw runtime_error{ "not init" };
                    }
                }
            }
        };

        template <typename E>
        class shared_state<void, E> : protected shared_state_base
        {
        private:
            variant<status_t, int, E> values_;

        public:
            static consteval size_t size() noexcept { return 0; }

            shared_state() : values_(status_t::none) {}

            void set_value()
            {
                unique_lock<mutex> lock{ mutex_ };
                values_ = 0;
                cond_var_.notify_one();
            }

            void set_error(E e)
            {
                unique_lock<mutex> lock{ mutex_ };
                values_ = move(e);
                cond_var_.notify_one();
            }

            void set_done()
            {
                unique_lock<mutex> lock{ mutex_ };
                values_ = status_t::done;
                cond_var_.notify_one();
            }

            void wait()
            {
                unique_lock<mutex> lock{ mutex_ };
                cond_var_.wait(lock, [this]{ return values_ != status_t::none; });

                auto const index = values_.index();
                if(index == 2)
                {
                    if constexpr(same_as<E, exception_ptr>)
                    {
                        rethrow_exception(get<2>(values_));
                    }
                    else
                    {
                        throw get<2>(values_);
                    }
                }
                else if(index == 0)
                {
                    if(get<0>(values_) == status_t::done)
                    {
                        terminate();
                    }
                    else
                    {
                        throw runtime_error{ "not init" };
                    }
                }
            }
        };

        template <typename ValueType, typename E = exception_ptr>
        class _receiver_type
        {
            using state_type = shared_state<ValueType>;

        public:
            static consteval size_t size() noexcept{ return state_type::size(); }

            _receiver_type()
                : state_(make_shared<shared_state<ValueType>>())
            {}
            ~_receiver_type() = default;

            _receiver_type(_receiver_type const&) = default;
            _receiver_type& operator= (_receiver_type const&) = default;
            _receiver_type(_receiver_type&&) = default;
            _receiver_type& operator= (_receiver_type&&) = default;

            template <typename ... Args>
            void set_value(Args&& ... args)
            {
                state_->set_value(forward<Args>(args)...);
            }

            void set_error(E&& e) noexcept
            {
                state_->set_error(forward<E>(e));
            }

            void set_done() noexcept
            {
                state_->set_done();
            }

            auto get_shared_state() const
            {
                return state_;
            }

        private:
            shared_ptr<state_type> state_;
        };

        template <typename Tuple, typename E>
        struct get_receiver_type;

        template <typename Arg, typename E>
        struct get_receiver_type<variant<tuple<Arg>>, E>
        {
            using type = _receiver_type<Arg, E>;
        };

        template <typename E>
        struct get_receiver_type<variant<tuple<>>, E>
        {
            using type = _receiver_type<void, E>;
        };

        template <typename ... Args, typename E> requires((sizeof...(Args) > 1))
        struct get_receiver_type<variant<tuple<Args...>>, E>
        {
            using type = _receiver_type<tuple<Args...>, E>;
        };

        template
        <
            template< template <typename ...> class Variant, template <typename ...> class Tuple> class ValueTypes,
            typename E = exception_ptr
        >
        using get_receiver_type_t = typename get_receiver_type<ValueTypes<variant, tuple>, E>::type;

        template <sender S>
        auto sync_wait(S&& s)
        {
            // get the type of recevier
            using receiver_type = get_receiver_type_t<sender_traits<remove_cvref_t<S>>::template value_types>;

            // construct a receiver
            receiver_type receiver{};

            // get the sync primitive in the receiver
            auto shared_state = receiver.get_shared_state();

            // start the async operation
            execution::start(execution::connect(forward<S>(s), move(receiver)));

            // wait on this thread
            shared_state->wait();

            // return value
            if constexpr(receiver_type::size() != 0)
            {
                return shared_state->values();
            }
        }
    }

    using sync_wait_n::sync_wait;
}