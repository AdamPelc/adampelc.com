#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <array>

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
    spsc_queue::queue_single_threaded_t<TypeParam, 4> queue;

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
    spsc_queue::queue_single_threaded_t<TypeParam, 4> queue;

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
    spsc_queue::queue_single_threaded_t<TypeParam, 4> queue{};

    auto enqueued_elements = std::vector{ TypeParam(0xDEAD'1), TypeParam(0xDEAD'2), TypeParam(0xDEAD'3), TypeParam(0xDEAD'4), TypeParam(0xDEAD'5) };
    auto expected_elements = std::vector{ TypeParam(0xDEAD'2), TypeParam(0xDEAD'3), TypeParam(0xDEAD'4), TypeParam(0xDEAD'5) };

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

// Edge cases and boundary conditions
TYPED_TEST(queue_single_threaded_test_t, wraparound_behavior_size_one) {
    spsc_queue::queue_single_threaded_t<TypeParam, 1> queue;

    // Multiple cycles of enqueue/dequeue to test wraparound
    for (int cycle = 0; cycle < 5; ++cycle) {
        auto element = TypeParam(0xDEAD + cycle);
        queue.enqueue(element);
        
        auto actual_element = TypeParam(0);
        const auto is_successful_dequeue = queue.try_dequeue(actual_element);
        
        ASSERT_TRUE(is_successful_dequeue);
        ASSERT_EQ(element, actual_element);
    }
}

TYPED_TEST(queue_single_threaded_test_t, mixed_enqueue_dequeue_operations) {
    spsc_queue::queue_single_threaded_t<TypeParam, 4> queue;
    
    // Pattern: enqueue 2, dequeue 1, enqueue 1, dequeue 2
    queue.enqueue(TypeParam(1));
    queue.enqueue(TypeParam(2));
    
    auto element = TypeParam(0);
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(1), element);
    
    queue.enqueue(TypeParam(3));
    
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(2), element);
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(3), element);
    
    ASSERT_FALSE(queue.try_dequeue(element));
}

TYPED_TEST(queue_single_threaded_test_t, partial_fill_and_drain_cycles) {
    spsc_queue::queue_single_threaded_t<TypeParam, 4> queue;
    
    // Fill partially, drain completely, repeat
    for (int cycle = 0; cycle < 3; ++cycle) {
        // Fill 2 elements
        queue.enqueue(TypeParam(cycle * 10 + 1));
        queue.enqueue(TypeParam(cycle * 10 + 2));
        
        // Drain all
        auto element = TypeParam(0);
        ASSERT_TRUE(queue.try_dequeue(element));
        ASSERT_EQ(TypeParam(cycle * 10 + 1), element);
        ASSERT_TRUE(queue.try_dequeue(element));
        ASSERT_EQ(TypeParam(cycle * 10 + 2), element);
        ASSERT_FALSE(queue.try_dequeue(element));
    }
}

TYPED_TEST(queue_single_threaded_test_t, exact_capacity_boundary) {
    spsc_queue::queue_single_threaded_t<TypeParam, 4> queue{};
    
    // Fill to exact capacity
    queue.enqueue(TypeParam(1));
    queue.enqueue(TypeParam(2));
    queue.enqueue(TypeParam(3));
    queue.enqueue(TypeParam(4));
    
    // One more should overwrite first
    queue.enqueue(TypeParam(5));
    
    // Should get elements 2, 3, 4, 5
    auto element = TypeParam(0);
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(2), element);
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(3), element);
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(4), element);
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(5), element);
    ASSERT_FALSE(queue.try_dequeue(element));
}

// Move-only type for testing
class move_only_t {
public:
    move_only_t() = default;
    explicit move_only_t(int value) : m_value(std::make_unique<int>(value)) {}
    
    // Delete copy operations
    move_only_t(const move_only_t&) = delete;
    move_only_t& operator=(const move_only_t&) = delete;
    
    // Allow move operations
    move_only_t(move_only_t&&) = default;
    move_only_t& operator=(move_only_t&&) = default;
    
    [[nodiscard]]
    friend bool operator==(const move_only_t& lhs, const move_only_t& rhs) {
        if (!lhs.m_value && !rhs.m_value) return true;
        if (!lhs.m_value || !rhs.m_value) return false;
        return *lhs.m_value == *rhs.m_value;
    }
    
    [[nodiscard]]
    int get_value() const { return m_value ? *m_value : 0; }

private:
    std::unique_ptr<int> m_value;
};

// Exception handling and move-only type tests
TEST(queue_single_threaded_move_only_test_t, move_only_enqueue_dequeue) {
    spsc_queue::queue_single_threaded_t<move_only_t, 2> queue{};
    
    auto element = move_only_t(42);
    queue.enqueue(std::move(element));
    
    auto result = move_only_t();
    const auto is_successful = queue.try_dequeue(result);
    
    ASSERT_TRUE(is_successful);
    ASSERT_EQ(42, result.get_value());
}

TEST(queue_single_threaded_move_only_test_t, move_only_overwrite_behavior) {
    spsc_queue::queue_single_threaded_t<move_only_t, 2> queue{};
    
    // Fill beyond capacity
    queue.enqueue(move_only_t(1));
    queue.enqueue(move_only_t(2));
    queue.enqueue(move_only_t(3)); // Should overwrite first
    
    // Should get 2, 3
    auto result = move_only_t();
    ASSERT_TRUE(queue.try_dequeue(result));
    ASSERT_EQ(2, result.get_value());
    ASSERT_TRUE(queue.try_dequeue(result));
    ASSERT_EQ(3, result.get_value());
    ASSERT_FALSE(queue.try_dequeue(result));
}

// State verification tests
TYPED_TEST(queue_single_threaded_test_t, dequeue_failure_preserves_output_parameter) {
    spsc_queue::queue_single_threaded_t<TypeParam, 2> queue;
    
    auto original_value = TypeParam(0xBEEF);
    auto element = original_value;
    
    const auto is_successful = queue.try_dequeue(element);
    
    ASSERT_FALSE(is_successful);
    ASSERT_EQ(original_value, element); // Should remain unchanged
}

TYPED_TEST(queue_single_threaded_test_t, queue_state_consistency_after_operations) {
    spsc_queue::queue_single_threaded_t<TypeParam, 4> queue;
    
    // Fill queue
    queue.enqueue(TypeParam(1));
    queue.enqueue(TypeParam(2));
    queue.enqueue(TypeParam(3));
    queue.enqueue(TypeParam(4));
    
    // Partial drain
    auto element = TypeParam(0);
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(1), element);
    
    // Add one more (should not overwrite)
    queue.enqueue(TypeParam(5));
    
    // Verify remaining elements in correct order
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(2), element);
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(3), element);
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(4), element);
    ASSERT_TRUE(queue.try_dequeue(element));
    ASSERT_EQ(TypeParam(5), element);
    ASSERT_FALSE(queue.try_dequeue(element));
}

TYPED_TEST(queue_single_threaded_test_t, alternating_discard_dequeue_consistency) {
    spsc_queue::queue_single_threaded_t<TypeParam, 4> queue;
    
    // Fill queue
    for (int i = 1; i <= 4; ++i) {
        queue.enqueue(TypeParam(i));
    }
    
    // Alternate between discard and dequeue
    ASSERT_TRUE(queue.try_discard()); // Discard 1
    
    auto element = TypeParam(0);
    ASSERT_TRUE(queue.try_dequeue(element)); // Dequeue 2
    ASSERT_EQ(TypeParam(2), element);
    
    ASSERT_TRUE(queue.try_discard()); // Discard 3
    
    ASSERT_TRUE(queue.try_dequeue(element)); // Dequeue 4
    ASSERT_EQ(TypeParam(4), element);
    
    ASSERT_FALSE(queue.try_dequeue(element));
    ASSERT_FALSE(queue.try_discard());
}

// Large object type for performance testing
struct large_object_t {
    std::array<int, 1024> data;
    int id;
    
    explicit large_object_t(int value = 0) : id(value) {
        data.fill(value);
    }
    
    [[nodiscard]]
    friend bool operator==(const large_object_t& lhs, const large_object_t& rhs) {
        return lhs.id == rhs.id && lhs.data == rhs.data;
    }
};

// Performance/behavior tests with different data types
TEST(queue_single_threaded_performance_test_t, large_object_operations) {
    spsc_queue::queue_single_threaded_t<large_object_t, 4> queue;
    
    // Test with large objects
    queue.enqueue(large_object_t(1));
    queue.enqueue(large_object_t(2));
    queue.enqueue(large_object_t(3));
    
    auto result = large_object_t();
    ASSERT_TRUE(queue.try_dequeue(result));
    ASSERT_EQ(large_object_t(1), result);
    ASSERT_TRUE(queue.try_dequeue(result));
    ASSERT_EQ(large_object_t(2), result);
    ASSERT_TRUE(queue.try_dequeue(result));
    ASSERT_EQ(large_object_t(3), result);
    ASSERT_FALSE(queue.try_dequeue(result));
}

TEST(queue_single_threaded_string_test_t, string_fifo_ordering) {
    spsc_queue::queue_single_threaded_t<std::string, 8> queue;
    
    std::vector<std::string> input_strings = {
        "first", "second", "third", "fourth", "fifth"
    };
    
    // Enqueue all strings
    for (const auto& str : input_strings) {
        queue.enqueue(str);
    }
    
    // Dequeue and verify order
    std::vector<std::string> output_strings;
    std::string result;
    while (queue.try_dequeue(result)) {
        output_strings.push_back(result);
    }
    
    ASSERT_EQ(input_strings, output_strings);
}

TEST(queue_single_threaded_string_test_t, string_overwrite_behavior) {
    spsc_queue::queue_single_threaded_t<std::string, 4> queue{};
    
    // Fill beyond capacity
    queue.enqueue(std::string("first"));
    queue.enqueue(std::string("second"));
    queue.enqueue(std::string("third"));
    queue.enqueue(std::string("fourth"));
    queue.enqueue(std::string("fifth"));   // Should overwrite "first"
    
    // Should get "second", "third", "fourth", "fifth"
    std::string result;
    ASSERT_TRUE(queue.try_dequeue(result));
    ASSERT_EQ("second", result);
    ASSERT_TRUE(queue.try_dequeue(result));
    ASSERT_EQ("third", result);
    ASSERT_TRUE(queue.try_dequeue(result));
    ASSERT_EQ("fourth", result);
    ASSERT_TRUE(queue.try_dequeue(result));
    ASSERT_EQ("fifth", result);
    ASSERT_FALSE(queue.try_dequeue(result));
}

TEST(queue_single_threaded_large_capacity_test_t, large_queue_behavior) {
    constexpr std::size_t large_size = 1024;
    spsc_queue::queue_single_threaded_t<int, large_size> queue;
    
    // Fill entire queue
    for (int i = 0; i < static_cast<int>(large_size); ++i) {
        queue.enqueue(i);
    }
    
    // Verify all elements in correct order
    int result;
    for (int i = 0; i < static_cast<int>(large_size); ++i) {
        ASSERT_TRUE(queue.try_dequeue(result));
        ASSERT_EQ(i, result);
    }
    
    ASSERT_FALSE(queue.try_dequeue(result));
}

