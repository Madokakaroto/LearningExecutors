#pragma once

namespace std::execution
{
    template <typename S, typename R>
    struct as_operation
    {
        remove_cvref_t<S> e_;
        remove_cvref_t<R> r_;

        void start() noexcept
        {
            try
            {
                execution::execute(std::move(e_), as_invocable<remove_cvref_t<R>, S>{ r_ });
            }
            catch(...)
            {
                execution::set_error(std::move(r_), current_exception());
            }
        }
    };
}