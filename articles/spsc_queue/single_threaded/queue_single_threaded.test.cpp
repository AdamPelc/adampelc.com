#include "queue_single_threaded.hpp"

#include <gtest/gtest.h>

class test_class_no_default_ctor_t {
public:
    test_class_no_default_ctor_t() = delete;
    constexpr explicit test_class_no_default_ctor_t(int value) : m_value(value) {}

    [[nodiscard]]
    friend bool operator==(const test_class_no_default_ctor_t& lhs, const test_class_no_default_ctor_t& rhs) {
        return lhs.m_value == rhs.m_value;
    }
private:
    int m_value;
};

template <typename element_T>
class queue_single_threaded_test_t : public ::testing::Test {};

using test_types_t = ::testing::Types<int, test_class_no_default_ctor_t>;
TYPED_TEST_SUITE(queue_single_threaded_test_t, test_types_t);

TYPED_TEST(queue_single_threaded_test_t, single_enqueue_dequeue) {// Arrange
    spsc_queue::queue_single_threaded_t<TypeParam, 1> queue;
    constexpr auto expected_element = TypeParam(0xDEAD);

    queue.enqueue(expected_element);
    const auto result = queue.try_dequeue();

    ASSERT_TRUE(result);
    ASSERT_EQ(expected_element, result.value());
}
