#pragma once

#include <bit>
#include <array>
#include <mutex>
#include <shared_mutex>

namespace spsc_queue {
    template<typename data_T, std::size_t size_T>
    class queue_locking_t {
    public:
        static_assert(size_T > 0, "Doesn't make sense");
        static_assert(std::popcount(size_T) == 1, "Size of the queue must be power of 2");

        queue_locking_t() = default;
        ~queue_locking_t();

        auto enqueue(data_T element) -> void;
        auto try_dequeue(data_T& element_out) -> bool;
        auto try_discard() -> bool;

    private:
        struct cell_t {
            alignas(data_T) std::byte m_buffer[sizeof(data_T)];
        };

        static constexpr std::size_t m_mask = size_T - 1;

        mutable std::mutex m_mutex;
        std::size_t m_write_idx = 0;
        std::size_t m_read_idx = 0;
        std::array<cell_t, size_T> m_cells;
    };

    template<typename data_T, std::size_t size_T>
    queue_locking_t<data_T, size_T>::~queue_locking_t() {
        while (this->try_discard()) {}
    }

    template<typename data_T, std::size_t size_T>
    auto queue_locking_t<data_T, size_T>::enqueue(data_T element) -> void {
        std::unique_lock lock(m_mutex);
        auto index = m_write_idx & m_mask;
        cell_t& cell = m_cells[index];
        const auto distance = m_write_idx - m_read_idx;
        if (distance >= size_T) {
            auto* old_element = reinterpret_cast<data_T*>(&cell.m_buffer);
            old_element->~data_T();
            ++m_read_idx;
        }
        new (cell.m_buffer) data_T(std::move(element));
        ++m_write_idx;
    }

    template<typename data_T, std::size_t size_T>
    auto queue_locking_t<data_T, size_T>::try_dequeue(data_T& element_out) -> bool {
        std::unique_lock lock(m_mutex);
        if (m_write_idx == m_read_idx) {
            return false;
        }

        const auto read_idx = m_read_idx & m_mask;

        auto* element = reinterpret_cast<data_T*>(m_cells[read_idx].m_buffer);
        element_out = std::move(*element);
        element->~data_T();
        ++m_read_idx;
        return true;
    }

    template<typename data_T, std::size_t size_T>
    auto queue_locking_t<data_T, size_T>::try_discard() -> bool {
        std::unique_lock lock(m_mutex);
        if (m_write_idx == m_read_idx) {
            return false;
        }

        const auto read_idx = m_read_idx & m_mask;
        auto* element = reinterpret_cast<data_T*>(m_cells[read_idx].m_buffer);
        element->~data_T();
        ++m_read_idx;
        return true;
    }
}
