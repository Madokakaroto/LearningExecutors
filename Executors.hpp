#pragma once

// algorithm
namespace std
{
    template <typename R, typename ... Args>
    inline auto set_value(R&& r, Args&& ... args)
        noexcept(noexcept(move(r).set_value(forward<Args>(args)...))) ->
        decltype(move(r).set_value(forward<Args>(args)...))
    {
        return move(r).set_value(forward<Args>(args)...);
    }

    template <typename R, typename E> requires
        requires(R&& r, E&& e) { { move(r).set_error(e) } noexcept; }
    inline decltype(auto) set_error(R&& r, E&& e) noexcept
    {
        return move(r).set_error(forward<E>(e));
    }

    template <typename R> requires
        requires(R&& r) { { move(r).set_done() } noexcept; }
    inline decltype(auto) set_done(R&& r) noexcept
    {
        return move(r).set_done();
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
            { set_done(move(r)) } noexcept;
            { set_error(move(r), forward<E>(e)) } noexcept;
        };

    template <typename R, typename ... Args>
    concept receiver_of =
        receiver<R> &&
        requires(remove_cvref_t<R>&& r, Args&& ... args)
        {
            set_value(move(r), forward<Args>(args)...);
        };

    template <typename R, typename ... Args>
    concept nothrow_receiver_of =
        receiver_of<R, Args...> &&
        requires(remove_cvref_t<R>&& r, Args&& ... args)
        {
            { set_value(move(r), forward<Args>(args)...) } noexcept;
        };

    template <typename R, typename ... Args>
    inline constexpr bool is_nothrow_receiver_of_v = nothrow_receiver_of<R, Args...>;
}