#pragma once

namespace std::execution
{
    template <typename R, typename E = exception_ptr>
    concept receiver =
        move_constructible<remove_cvref_t<R>> &&
        constructible_from<remove_cvref_t<R>, R> &&
        requires(R&& r, E&& e)
        {
            { execution::set_done(forward<R>(r)) } noexcept;
            { execution::set_error(forward<R>(r), forward<E>(e)) } noexcept;
        };

    template <typename R, typename ... Args>
    concept receiver_of =
        receiver<R> &&
        requires(R&& r, Args&& ... args)
        {
            execution::set_value(forward<R>(r), forward<Args>(args)...);
        };

    template <typename R, typename ... Args>
    concept nothrow_receiver_of =
        receiver_of<R, Args...> &&
        is_nothrow_invocable_v<decltype(set_value), Args...>;

    template <typename R, typename ... Args>
    inline constexpr bool is_nothrow_receiver_of_v = nothrow_receiver_of<R, Args...>;
}