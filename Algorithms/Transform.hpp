#pragma once

namespace std::execution
{
    namespace transform_n
    {
        template <receiver R, typename F>
        struct _receiver_type
        {
            R r_;
            F f_;

            // Customize set_value by invoking the callable and passing the result to the base class
            template <typename ... Args> requires receiver_of<R, invoke_result_t<F, Args...>>
            void set_value(Args&& ... args) &&
                noexcept(is_nothrow_invocable_v<F, Args...> && is_nothrow_receiver_of_v<R, invoke_result_t<F, Args...>>)
            {
                execution::set_value(move(r_), invoke(move(f_), forward<Args>(args)...));
            }

            template <typename ... Args> requires receiver_of<R> && is_void_v<invoke_result_t<F, Args...>>
            void set_value(Args&& ... args) &&
                noexcept(is_nothrow_invocable_v<F, Args...> && is_nothrow_receiver_of_v<R>)
            {
                invoke(move(f_), forward<Args>(args)...);
                execution::set_value(move(r_));
            }

            template <typename E> requires receiver<R, remove_cvref_t<E>>
            void set_error(E&& e) && noexcept
            {
                execution::set_error(move(r_), forward<E>(e));
            }

            void set_done() && noexcept
            {
                execution::set_done(move(r_));
            }
        };

        template <typename F, typename Param>
        struct transform_invoke_result
        {
            template
            <
                template <typename ...> class Variant,
                template <typename ...> class Tuple
            >
            using apply = Variant<Tuple<invoke_result_t<F, Param>>>;
        };

        template <typename F>
        struct transform_invoke_result<F, void>
        {
            template
            <
                template <typename ...> class Variant,
                template <typename ...> class Tuple
            >
            using apply = Variant<Tuple<invoke_result_t<F>>>;
        };

        template <typename F, typename ... Args>
        struct transform_invoke_result<F, tuple<Args...>>
        {
            template
            <
                template <typename ...> class Variant,
                template <typename ...> class Tuple
            >
            using apply = Variant<Tuple<invoke_result_t<F, Args...>>>;
        };

        template <sender S, typename F>
        struct _sender_type
        {
            S s_;
            F f_;

            // traits out the value types by std::variant and std::tuple
            using precede_value_types = get_value_types_t<typename sender_traits<S>::template value_types<variant, tuple>>;

            template
            <
                template <typename ...> class Variant,
                template <typename ...> class Tuple
            >
            using value_types = typename transform_invoke_result<F, precede_value_types>::template apply<Variant, Tuple>;

            // TODO ... a better error types
            template
            <
                template <typename ...> class Variant
            >
            using error_types = typename sender_traits<S>::template error_types<Variant>;

            static constexpr bool sends_done = sender_traits<S>::sends_done;

            template <receiver R> requires(sender_to<S, _receiver_type<remove_cvref_t<R>, F>>)
            auto connect(R&& r) && -> connect_result_t<S, _receiver_type<remove_cvref_t<R>, F>>
            {
                using receiver_type = _receiver_type<remove_cvref_t<R>, F>;
                return execution::connect(move(s_), receiver_type{ forward<R>(r), move(f_) });
            }
        };

        template <typename S, typename F>
        concept customise_point =
            requires(S&& s, F&& f)
            {
                then(forward<S>(s), forward<F>(f));
            };

        struct func_type
        {
            // default implementation no constraints
            template <typename S, typename F>
            auto operator()(S&& s, F&& f) const noexcept
            {
                using sender_type = _sender_type<remove_cvref_t<S>, remove_cvref_t<F>>;
                return sender_type{ forward<S>(s), forward<F>(f) };
            }

            template <typename S, typename F> requires customise_point<S, F>
            decltype(auto) operator()(S&& s, F&& f) const noexcept(noexcept(transform(declval<S>(), declval<F>())))
            {
                return transform(forward<S>(s), forward<F>(f));
            }

            template <typename F>
            auto operator() (F&& f) const noexcept
            {
                return _pipe_operand_type{ *this, forward<F>(f) };
            }
        };
    }

    inline constexpr transform_n::func_type transform{};
}