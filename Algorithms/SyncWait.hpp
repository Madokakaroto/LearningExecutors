#pragma once

namespace std::execution
{
    namespace sync_wait_n
    {
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