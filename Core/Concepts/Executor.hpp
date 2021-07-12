#pragma once

namespace std::execution
{
    template <typename E, typename F>
    concept executor_of_impl =
        invocable<add_lvalue_reference_t<remove_cvref_t<F>>> &&
        constructible_from<remove_cvref_t<F>, F> &&
        move_constructible<remove_cvref_t<F>> &&
        copy_constructible<E> &&
        is_nothrow_copy_constructible_v<E> &&
        equality_comparable<E> &&
        requires(E&& e, F&& f)
        {
            execution::execute(forward<E>(e), forward<F>(f));
        };

    template <typename E>
    concept executor = executor_of_impl<E, invocable_archetype>;

    namespace as_operation_cpo
    {
        template <typename E, typename R>
        concept as_operation_impl =
            negation_v<is_instance_of<remove_cvref_t<R>, as_invocable>> &&
            receiver_of<R> &&
            executor_of_impl<remove_cvref_t<E>, as_invocable<remove_cvref_t<R>, E>>;

        template <typename E, typename R>
        struct _op
        {
            remove_cvref_t<E> e_;
            remove_cvref_t<R> r_;

            void start() noexcept
            {
                try
                {
                    execution::execute(std::move(e_), as_invocable<remove_cvref_t<R>, E>{ r_ });
                }
                catch(...)
                {
                    execution::set_error(std::move(r_), current_exception());
                }
            }
        };

        template <typename E, typename R> requires(as_operation_impl<E, R>)
        inline auto connect(E&& e, R&& r) noexcept -> _op<E, R>
        {
            return _op{ forward<E>(e), forward<R>(r) };
        }
    }
}