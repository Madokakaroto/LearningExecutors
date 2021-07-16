#pragma once

namespace std::execution
{
    namespace immediate_n
    {
        template <typename R> requires receiver_of<R>
        struct _op
        {
            R r_;

            void start() && noexcept
            {
                try
                {
                    execution::set_value(move(r_));
                }
                catch (...)
                {
                    execution::set_error(move(r_), current_exception());
                }
            }
        };

        struct _sender_type
        {
            template
            <
                template <typename ...> class Variant,
                template <typename ...> class Tuple
            >
            using value_types = Variant<Tuple<>>;

            template
            <
                template <typename ...> class Variant
            >
            using error_types = Variant<exception_ptr>;

            static constexpr bool sends_done = true;

            template <typename R> requires(receiver_of<R>)
            auto connect(R&& r) const noexcept
            {
                return _op<remove_cvref_t<R>>{ forward<R>(r) };
            }

            template <typename R> requires(receiver_of<R>)
            void submit(R&& r) const
            {
                execution::start(execution::connect(*this, forward<R>(r)));
            }
        };

        struct _scheduler_type
        {
            // operator (not) equal compare
            friend bool operator ==(_scheduler_type const& lhs, _scheduler_type const& rhs) noexcept
            {
                return true;
            }

            friend bool operator !=(_scheduler_type const& lhs, _scheduler_type const& rhs) noexcept
            {
                return false;
            }

            _sender_type schedule() const noexcept
            {
                return {};
            }
        };
    }

    inline constexpr struct immediate_context_t
    {
        immediate_n::_scheduler_type get_scheduler() const noexcept
        {
            return {};
        }
    } immediate_context{};
}