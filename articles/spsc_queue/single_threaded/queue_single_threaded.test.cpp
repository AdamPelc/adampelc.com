#include <algorithm>
#include <vector>

#include "queue_single_threaded.hpp"

#include <gtest/gtest.h>

class test_class_no_default_ctor_t {
public:
    test_class_no_default_ctor_t() = delete;
    constexpr explicit test_class_no_default_ctor_t(int value) : m_value(std::make_shared<int>(value)) {}

    [[nodiscard]]
    friend bool operator==(const test_class_no_default_ctor_t& lhs, const test_class_no_default_ctor_t& rhs) {
        return *lhs.m_value == *rhs.m_value;
    }
private:
    std::shared_ptr<int> m_value;
};

template <typename element_T>
class queue_single_threaded_test_t : public ::testing::Test {};

using test_types_t = ::testing::Types<int, test_class_no_default_ctor_t>;
TYPED_TEST_SUITE(queue_single_threaded_test_t, test_types_t);

TYPED_TEST(queue_single_threaded_test_t, single_enqueue_dequeue) {
    spsc_queue::queue_single_threaded_t<TypeParam, 1> queue;
    auto expected_element = TypeParam(0xDEAD);

    // Act
    queue.enqueue(expected_element);
    auto actual_element = TypeParam(0);
    const auto is_successful_dequeue = queue.try_dequeue(actual_element);

    ASSERT_TRUE(is_successful_dequeue);
    ASSERT_EQ(expected_element, actual_element);
}

TYPED_TEST(queue_single_threaded_test_t, single_enqueue_single_discard) {
    spsc_queue::queue_single_threaded_t<TypeParam, 1> queue;
    auto expected_element = TypeParam(0xDEAD);

    // Act
    queue.enqueue(expected_element);
    const auto is_successful_discard = queue.try_discard();

    ASSERT_TRUE(is_successful_discard);
}

TYPED_TEST(queue_single_threaded_test_t, single_enqueue_dual_discard) {
    spsc_queue::queue_single_threaded_t<TypeParam, 1> queue;
    auto expected_element = TypeParam(0xDEAD);

    // Act
    queue.enqueue(expected_element);
    queue.try_discard();

    const auto is_successful_discard = queue.try_discard();
    ASSERT_FALSE(is_successful_discard);
}

TYPED_TEST(queue_single_threaded_test_t, dequeue_empty) {
    spsc_queue::queue_single_threaded_t<TypeParam, 1> queue;

    auto actual_element = TypeParam(0);
    const auto is_successful_dequeue = queue.try_dequeue(actual_element);

    ASSERT_FALSE(is_successful_dequeue);
}

TYPED_TEST(queue_single_threaded_test_t, discard_empty) {
    spsc_queue::queue_single_threaded_t<TypeParam, 1> queue;

    const auto is_successful_discard = queue.try_discard();
    ASSERT_FALSE(is_successful_discard);
}

TYPED_TEST(queue_single_threaded_test_t, muliple_enqueue_all_dequeue) {
    spsc_queue::queue_single_threaded_t<TypeParam, 3> queue;

    auto expected_elements = std::vector{ TypeParam(0xDEAD'1), TypeParam(0xDEAD'2), TypeParam(0xDEAD'3) };

    for (const auto& element : expected_elements) {
        queue.enqueue(element);
    }

    std::vector<TypeParam> actual_elements(std::size(expected_elements), TypeParam(0));
    for (auto& actual_element : actual_elements) {
        const auto is_successful_dequeue = queue.try_dequeue(actual_element);
        ASSERT_TRUE(is_successful_dequeue);
    }

    ASSERT_EQ(expected_elements, actual_elements);
}

TYPED_TEST(queue_single_threaded_test_t, muliple_enqueue_all_discard) {
    spsc_queue::queue_single_threaded_t<TypeParam, 3> queue;

    auto expected_elements = std::vector{ TypeParam(0xDEAD'1), TypeParam(0xDEAD'2), TypeParam(0xDEAD'3) };

    for (const auto& element : expected_elements) {
        queue.enqueue(element);
    }

    for (const auto& _ : expected_elements) {
        const auto is_successful_discard = queue.try_discard();
        ASSERT_TRUE(is_successful_discard);
    }

    const auto is_successful_discard = queue.try_discard();
    ASSERT_FALSE(is_successful_discard);
}

TYPED_TEST(queue_single_threaded_test_t, multiple_enqueue_all_dequeue_oversize) {
    spsc_queue::queue_single_threaded_t<TypeParam, 2> queue;

    auto enqueued_elements = std::vector{ TypeParam(0xDEAD'1), TypeParam(0xDEAD'2), TypeParam(0xDEAD'3) };
    auto expected_elements = std::vector{ TypeParam(0xDEAD'2), TypeParam(0xDEAD'3) };

    for (const auto& element : enqueued_elements) {
        queue.enqueue(element);
    }

    std::vector<TypeParam> actual_elements(std::size(expected_elements), TypeParam(0));
    for (auto& actual_element : actual_elements) {
        const auto is_successful_dequeue = queue.try_dequeue(actual_element);
        ASSERT_TRUE(is_successful_dequeue);
    }

    ASSERT_EQ(expected_elements, actual_elements);
}

TYPED_TEST(queue_single_threaded_test_t, multiple_enqueue_all_discard_oversize) {
    spsc_queue::queue_single_threaded_t<TypeParam, 2> queue;

    auto enqueued_elements = std::vector{ TypeParam(0xDEAD'1), TypeParam(0xDEAD'2), TypeParam(0xDEAD'3) };
    auto expected_elements = std::vector{ TypeParam(0xDEAD'2), TypeParam(0xDEAD'3) };

    for (const auto& element : enqueued_elements) {
        queue.enqueue(element);
    }

    for (const auto& _ : expected_elements) {
        const auto is_successful_dequeue = queue.try_discard();
        ASSERT_TRUE(is_successful_dequeue);
    }

    const auto is_successful_discard = queue.try_discard();
    ASSERT_FALSE(is_successful_discard);
}

