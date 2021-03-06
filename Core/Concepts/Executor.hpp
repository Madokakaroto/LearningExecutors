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
        is_execute_invocable_v<E, F>;

    template <typename E>
    concept executor = executor_of_impl<E, invocable_archetype>;
}