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

int main(void)
{
    std::execution::sender_traits<test_sender_traits>::sends_done;
    return 0;
}