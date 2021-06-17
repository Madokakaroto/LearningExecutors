#pragma once

namespace std::execution
{
    template <typename E>
    class as_sender
    {
    private:
        E ex_;

    public:
        template <
            template <typename...> class Tuple,
            template <typename...> class Variant >
        using value_types = Variant<Tuple<>>;

        template <template <typename...> class Variant>
        static constexpr bool sends_done = true;

        explicit as_sender(E e) noexcept
            : ex_(move(e)) {}

        template <typename R> requires receiver_of<R>
        connect_result_t<E, R> connect(R&& r) &&
        {
            return execution::connect(move(ex_), forward<R>(r));
        }

        template <typename R> requires receiver_of<R>
        connect_result_t<E const&, R> connect(R&& r) const &
        {
            return execution::connect(ex_, forward<R>(r));
        }
    };

    namespace schedule_n
    {
        template <typename S>
        concept default_impl =
            requires(S&& s)
            {
                { forward<S>(s).schedule() } -> sender;
            };

        template <typename S>
        concept customise_point =
            requires(S&& s)
            {
                { schedule(forward<S>(s)) } -> sender;
            };

        struct func_type
        {
            template <typename S> requires(default_impl<S> && !customise_point<S>)
            decltype(auto) operator() (S&& s) const
                noexcept(noexcept(forward<S>(s).schedule()))
            {
                return forward<S>(s).schedule();
            }

            template <typename S> requires(customise_point<S>)
            decltype(auto) operator() (S&& s) const
                noexcept(noexcept(schedule(forward<S>(s))))
            {
                return schedule(forward<S>(s));
            }

            template <typename E> requires(executor<E> && !(default_impl<E> && customise_point<E>))
            decltype(auto) operator() (E&& e) const
                noexcept(is_nothrow_constructible_v<as_sender<remove_cvref_t<E>>, E&&>)
            {
                return as_sender<remove_cvref_t<E>>{ forward<E>(e) };
            }
        };

        inline constexpr func_type schedule{};
    }

    using schedule_n::schedule;
}