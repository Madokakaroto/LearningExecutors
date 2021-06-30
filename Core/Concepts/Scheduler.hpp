#pragma once

namespace std::execution
{
    template <typename S>
    concept scheduler =
        copy_constructible<remove_cvref_t<S>> &&
        equality_comparable<remove_cvref_t<S>> &&
        requires(S&& s)
        {
            execution::schedule(forward<S>(s));
        };
}