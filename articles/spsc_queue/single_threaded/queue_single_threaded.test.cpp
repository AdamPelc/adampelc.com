#include <algorithm>
#include <vector>

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

TYPED_TEST(queue_single_threaded_test_t, single_enqueue_dequeue) {
    spsc_queue::queue_single_threaded_t<TypeParam, 1> queue;
    constexpr auto expected_element = TypeParam(0xDEAD);

    queue.enqueue(expected_element);
    const auto result = queue.try_dequeue();

    ASSERT_TRUE(result);
    ASSERT_EQ(expected_element, result.value());
}

TYPED_TEST(queue_single_threaded_test_t, dequeue_empty) {
    spsc_queue::queue_single_threaded_t<TypeParam, 1> queue;

    const auto result = queue.try_dequeue();

    ASSERT_EQ(std::nullopt, result);
}

TYPED_TEST(queue_single_threaded_test_t, muliple_enqueue_all_dequeue) {
    spsc_queue::queue_single_threaded_t<TypeParam, 3> queue;

    auto expected_elements = std::vector{ TypeParam(0xDEAD'1), TypeParam(0xDEAD'2), TypeParam(0xDEAD'3) };

    for (const auto& element : expected_elements) {
        queue.enqueue(element);
    }

    std::vector<TypeParam> actual_elements;
    actual_elements.reserve(expected_elements.size());
    while (const auto element = queue.try_dequeue()) {
        actual_elements.push_back(element.value());
    }

    ASSERT_EQ(expected_elements, actual_elements);
}

TYPED_TEST(queue_single_threaded_test_t, multiple_enqueue_all_dequeue_oversize) {
    spsc_queue::queue_single_threaded_t<TypeParam, 2> queue;

    auto enqueued_elements = std::vector{ TypeParam(0xDEAD'1), TypeParam(0xDEAD'2), TypeParam(0xDEAD'3) };
    auto expected_elements = std::vector{ TypeParam(0xDEAD'2), TypeParam(0xDEAD'3) };

    for (const auto& element : enqueued_elements) {
        queue.enqueue(element);
    }

    std::vector<TypeParam> actual_elements;
    actual_elements.reserve(expected_elements.size());
    while (const auto element = queue.try_dequeue()) {
        actual_elements.push_back(element.value());
    }

    ASSERT_EQ(expected_elements, actual_elements);
}
