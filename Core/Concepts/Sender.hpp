#pragma once

namespace std::execution
{
    template <typename S>
    struct test_atomic_c
    {
        constexpr operator bool ()const{ return true; }
        static constexpr bool value() noexcept
        { 
            return static_cast<bool>(!is_base_of_v<invalid_sender_traits, sender_traits<remove_cvref_t<S>>>);
        }
    };

    template <typename S>
    concept sender =
        move_constructible<remove_cvref_t<S>> &&
        test_atomic_c<S>::value();

    template <typename S, typename R>
    concept sender_to =
        sender<S> &&
        receiver<R> &&
        is_connect_invocable_v<S, R>;

    // A sender is typed if it declares what types it sends through a receiver’s channels.
    template <typename S>
    concept typed_sender = sender<S> && has_sender_types<sender_traits<remove_cvref_t<S>>>;
}