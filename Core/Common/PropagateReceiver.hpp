#pragma once

namespace std::execution
{
    template <typename S, typename R>
    struct _propagate_receiver
    {
        S s_;
        R r_;

        _propagate_receiver(S&& s, R&& r)
            : s_(move(s))
            , r_(move(r))
        {}

        template <typename ... Args>
        void set_value(Args&& ... args) && noexcept
        {
            try
            {
                _value_receiver<R, Args...> r{ move(r_), forward<Args>(args)... };
                execution::submit(move(s_), move(r));
            }
            catch(...)
            {
                execution::set_error(move(r_), current_exception());
            }
        }

        template <typename E>
        void set_error(E&& e) && noexcept
        {
            try
            {
                _error_receiver r{ move(r_), forward<E> (e) };
                execution::submit(move(s_), move(r));
            }
            catch(...)
            {
                execution::set_error(move(r_), current_exception());
            }
        }

        void set_done() && noexcept
        {
            try
            {
                _done_receiver r{ move(r_) };
                execution::submit(move(s_), move(r));
            }
            catch(...)
            {
                execution::set_error(move(r_), current_exception());
            }
        }
    };

    template <typename S1, typename S2>
        requires(sender<S1>, sender_to<S2, void_receiver>)
    struct _propagate_sender
    {
        S1 s1_;
        S2 s2_;

        template
        <
            template <typename ...> class Variant,
            template <typename ...> class Tuple
        >
        using value_types = typename sender_traits<S1>::template value_types<Variant, Tuple>;

        template
        <
            template <typename ...> class Variant
        >
        using error_types = typename sender_traits<S1>::template error_types<Variant>;

        static constexpr bool sends_done =
            sender_traits<S1>::sends_done ||
            sender_traits<S2>::sends_done;

        _propagate_sender(S1&& s1, S2&& s2)
            : s1_(move(s1))
            , s2_(move(s2))
        {}

        template <typename R> requires(sender_to<S1, R>)
        auto connect(R&& r) && noexcept(is_nothrow_invocable_v<decltype(execution::connect), S1, _propagate_receiver<S2, R>>)
        {
            _propagate_receiver pr{ move(s2_), forward<R>(r) };
            return execution::connect(move(s1_), move(pr));
        }
    };
}