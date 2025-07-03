#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>

namespace spsc_queue {
    template<typename data_T, std::size_t size_T>
    class queue_single_threaded_t {
    public:
        static_assert(size_T > 0, "Doesn't make sense");

        auto enqueue(data_T element) -> void;
        auto try_dequeue() -> std::optional<data_T>;

    private:
        struct cell_t {
            alignas(data_T) std::byte m_buffer[sizeof(data_T)];
        };
        std::size_t m_write_idx = 0;
        std::size_t m_read_idx = 0;
        std::array<cell_t, size_T> m_cells;
    };

    template<typename data_T, std::size_t size_T>
    auto queue_single_threaded_t<data_T, size_T>::enqueue(data_T element) -> void {
        auto index = m_write_idx % size_T;
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
    auto queue_single_threaded_t<data_T, size_T>::try_dequeue() -> std::optional<data_T> {
        if (m_write_idx == m_read_idx) {
            return std::nullopt;
        }

        const auto read_idx = m_read_idx % size_T;

        auto* element = reinterpret_cast<data_T*>(m_cells.at(read_idx).m_buffer);
        auto result = std::move(*element);
        element->~data_T();
        ++m_read_idx;
        return result;
    }
}
