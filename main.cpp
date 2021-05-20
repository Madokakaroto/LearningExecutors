#include "Forward.hpp"
#include "Concepts.hpp"
#include "Executors.hpp"

struct test_sender_traits : std::execution::sender_base
{
    template <template <typename...> class Tuple, template <typename...> class Variant>
    using value_types = Variant<Tuple<>>;

    template <template <typename...> class Variant>
    using error_types = Variant<std::exception_ptr>;

    static constexpr bool sends_done = true;
};

struct test_set_value_both
{
    friend void set_value(test_set_value_both&& v, int i) noexcept
    {
        v.value_ = i + 1;
    }

    void set_value(int i) noexcept
    {
        this->value_ = i;
    }

    int value_{ 0 };
};

struct test_set_value_custom
{
    friend void set_value(test_set_value_custom&& v, int i) noexcept
    {
        v.value_ = i + 1;
    }

    int value_{ 0 };
};

struct test_set_value_default
{
    void set_value(int i) noexcept
    {
        this->value_ = i;
    }

    int value_{ 0 };
};

int main(void)
{
    static_assert(std::execution::sender_traits<test_sender_traits>::sends_done);
    return 0;

    test_set_value_both both_v{};
    test_set_value_custom custom_v{};
    test_set_value_default default_v{};

    std::execution::set_value(both_v, 1);
    std::execution::set_value(custom_v, 1);
    std::execution::set_value(default_v, 1);
}