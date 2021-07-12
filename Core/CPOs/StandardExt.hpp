#pragma once

namespace std::execution
{
    namespace ext
    {
        template <typename E, typename R>
        concept as_operation_impl =
            negation_v<is_instance_of<remove_cvref_t<R>, as_invocable>> &&
            receiver_of<R> &&
            executor_of_impl<remove_cvref_t<E>, as_invocable<remove_cvref_t<R>, E>>;

        template <typename E, typename R>
        struct _as_operation_op
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
    }

    namespace connect_n
    {
        template <typename E, typename R> requires(ext::as_operation_impl<E, R>)
        inline auto connect(E&& e, R&& r) noexcept -> ext::_as_operation_op<E, R>
        {
            return ext::_as_operation_op{ forward<E>(e), forward<R>(r) };
        }
    }
}