#pragma once

namespace std::execution
{
    template <typename S, typename R>
    concept sender_to =
        sender<S> &&
        receiver<R> &&
        requires(S&& s, R&& r)
        {
            execution::connect(forward<S>(s), forward<R>(r));
        };
}