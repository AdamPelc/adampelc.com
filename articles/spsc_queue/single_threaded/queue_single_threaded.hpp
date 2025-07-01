#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>

namespace spsc_queue {
    template<typename data_T, std::size_t size_T>
    class queue_single_threaded_t {
    public:
        auto enqueue(data_T element) -> void;
        auto try_dequeue() -> std::optional<data_T>;

    private:
        alignas(data_T) std::byte m_element[sizeof(data_T)] = {};
    };

    template<typename data_T, std::size_t size_T>
    auto queue_single_threaded_t<data_T, size_T>::enqueue(data_T element) -> void {
        new (&m_element) data_T(std::move(element));
    }

    template<typename data_T, std::size_t size_T>
    auto queue_single_threaded_t<data_T, size_T>::try_dequeue() -> std::optional<data_T> {
        auto* element = reinterpret_cast<data_T*>(&m_element);
        auto result = std::move(*element);
        element->~data_T();
        return result;
    }
}
