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

    // A sender is typed if it declares what types it sends through a receiver’s channels.
    template <typename S>
    concept typed_sender = sender<S> && has_sender_types<sender_traits<remove_cvref_t<S>>>;
}