#pragma once

// algorithm
namespace std
{
    template <typename R, typename ... Args>
    inline auto set_value(R&& r, Args&& ... args) -> decltype(std::move(r).set_value(std::forward<Args>(args)...))
    {
        return std::move(r).set_value(std::forward<Args>(args)...);
    }
}

namespace std
{
    template <typename R, typename E = bad_exception>
    concept receiver =
        move_constructible<remove_cvref_t<R>> &&
        constructible_from<remove_cvref_t<R>, R> &&
        requires(remove_cvref_t<R>&& r, E&& e)
        {
            { set_done(std::move(r)) } noexcept;
            { set_error(std::move(r), std::forward<E>(e)) } noexcept;
        };

    template <typename R, typename ... Args>
    concept receiver_of =
        receiver<R> &&
        requires(remove_cvref_t<R>&& r, Args&& ... args)
        {
            set_value(std::move(r), std::forward<Args>(args)...);
        };

    template <typename R, typename ... Args>
    concept nothrow_receiver_of =
        receiver_of<R, Args...> &&
        requires(remove_cvref_t<R>&& r, Args&& ... args)
        {
            { set_value(std::move(r), std::forward<Args>(args)...) } noexcept;
        };

    template <typename R, typename ... Args>
    inline constexpr bool is_nothrow_receiver_of_v = nothrow_receiver_of<R, Args...>;
}