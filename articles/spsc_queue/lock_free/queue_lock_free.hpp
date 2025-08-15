#pragma once

#include <bit>
#include <array>
#include <atomic>

namespace spsc_queue {
    template<typename data_T, std::size_t size_T>
    class queue_lock_free_t {
    public:
        static_assert(size_T >= 2, "Queue size must be at least 2");
        static_assert(std::has_single_bit(size_T), "Size of the queue must be power of 2");

        queue_lock_free_t() = default;
        ~queue_lock_free_t();

        auto enqueue(data_T element) -> void;
        auto try_dequeue(data_T& element_out) -> bool;
        auto try_discard() -> bool;

    private:
        struct alignas(64) cell_t {
            alignas(data_T) std::byte m_buffer[sizeof(data_T)];
        };

        static constexpr std::size_t m_mask = size_T - 1;

        alignas(64) std::atomic_size_t m_write_idx = 0;
        alignas(64) std::atomic_size_t m_read_idx = 0;
        alignas(64) std::atomic_flag m_cell_lock = ATOMIC_FLAG_INIT;
        alignas(64) std::array<cell_t, size_T> m_cells;
    };

    template<typename data_T, std::size_t size_T>
    queue_lock_free_t<data_T, size_T>::~queue_lock_free_t() {
        while (this->try_discard()) {}
    }

    template<typename data_T, std::size_t size_T>
    auto queue_lock_free_t<data_T, size_T>::enqueue(data_T element) -> void {
        // Get the current write position
        const auto write_idx = m_write_idx.load(std::memory_order_relaxed);
        auto read_idx = m_read_idx.load(std::memory_order_acquire);

        const auto index = write_idx & m_mask;
        if (write_idx - read_idx == size_T) {

            // Wait for free cell
            while (m_cell_lock.test_and_set(std::memory_order_acquire)) {}

            // Check if element to be overridden has not been already dequeued.
            if (m_read_idx.compare_exchange_weak(read_idx, read_idx + 1, std::memory_order_release, std::memory_order_relaxed)) {
                // It means that element has to be removed before new one will be emplaced
                auto* old_element = reinterpret_cast<data_T*>(m_cells[index].m_buffer);
                old_element->~data_T();
            }
            // Add new element
            new (m_cells[index].m_buffer) data_T(std::move(element));
            m_cell_lock.clear(std::memory_order_release);
        }
        else {
            // There is free space in buffer to add element
            new (m_cells[index].m_buffer) data_T(std::move(element));
        }

        // Advance write index
        m_write_idx.store(write_idx + 1, std::memory_order_release);
    }

    template<typename data_T, std::size_t size_T>
    auto queue_lock_free_t<data_T, size_T>::try_dequeue(data_T& element_out) -> bool {
        auto read_idx = m_read_idx.load(std::memory_order_relaxed);
        const auto write_idx = m_write_idx.load(std::memory_order_acquire);
        if (read_idx == write_idx) {
            return false;
        }

        // Following is required for synchronization in forced enqueue
        if (m_cell_lock.test_and_set(std::memory_order_acquire)) {
            return false;
        }

        read_idx = m_read_idx.load(std::memory_order_acquire);
        const auto index = read_idx & m_mask;
        auto* element = reinterpret_cast<data_T*>(m_cells[index].m_buffer);
        element_out = std::move(*element);
        element->~data_T();

        m_read_idx.store(read_idx + 1, std::memory_order_release);
        m_cell_lock.clear(std::memory_order_release);

        return true;
    }

    template<typename data_T, std::size_t size_T>
    auto queue_lock_free_t<data_T, size_T>::try_discard() -> bool {
        // Load current read index
        const auto read_idx = m_read_idx.load(std::memory_order_relaxed);
        // Use acquire to see producer's latest write_idx updates
        const auto write_idx = m_write_idx.load(std::memory_order_acquire);

        if (read_idx >= write_idx) {
            return false;  // Empty queue
        }

        const auto index = read_idx & m_mask;
        auto* element = reinterpret_cast<data_T*>(m_cells[index].m_buffer);
        element->~data_T();

        // Use release to make our read_idx update visible to producer
        m_read_idx.store(read_idx + 1, std::memory_order_release);
        return true;
    }
}