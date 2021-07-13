#include <iostream>
#include "../Executors.hpp"

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
    friend void set_value(test_set_value_both& v, int i) noexcept
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
    friend void set_value(test_set_value_custom& v, int i) noexcept
    {
        v.value_ = i;
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

inline constexpr struct _sink {
    void set_value(auto&&...) const noexcept {}
    [[noreturn]] void set_error(auto&&) const noexcept {
        std::terminate();
    }
    [[noreturn]] void set_done() const noexcept {
        std::terminate();
    }
} sink{};

int main(void)
{
    static_assert(std::execution::sender_traits<test_sender_traits>::sends_done);

    test_set_value_both both_v{};
    test_set_value_custom custom_v{};
    test_set_value_default default_v{};

    std::execution::set_value(custom_v, 1);
    //exec::set_value(1, 1);

    auto just_sender = std::execution::just(1, 2, 3);
    auto f = [](int i, int j, int k) noexcept
    {
        printf("i=%d, j=%d, k=%d\n", i, j, k);
    };

    std::execution::start(std::execution::connect(std::execution::transform(just_sender, f), sink));


    /*std::execution::static_thread_pool pool{ 4 };
    std::execution::on(std::execution::just(2), pool.get_scheduler());
    std::execution::just_on(pool.get_scheduler(), 2);
    std::execution::just(2)
        | std::execution::on(pool.get_scheduler())
        | std::execution::transform([](int i){ return static_cast<double>(i) * 2.0; });*/

    auto [a, b] = std::execution::sync_wait(std::execution::just(2, 3));
    printf("a=%d, b=%d\n", a, b);

    return 1;
}