#pragma once

namespace std::execution
{
    template <typename S>
    concept sender =
        move_constructible<remove_cvref_t<S>> &&
        !requires
        {
            typename sender_traits<remove_cvref_t<S>>::__unspecialized;
        };

    template <typename S, typename R>
    concept sender_to =
        sender<S> &&
        receiver<R> &&
        requires(S&& s, R&& r)
        {
            execution::connect(move(s), move(r));
        };
}