#pragma once

#include <array>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>

namespace adas {
template<typename T, std::size_t Capacity>
class BoundedQueue {
public:
    bool try_push(T value) {
        std::lock_guard<std::mutex> lock{mutex_};
        if (closed_ || size_ == Capacity) return false;
        data_[head_] = std::move(value);
        head_ = (head_ + 1u) % Capacity;
        ++size_;
        ready_.notify_one();
        return true;
    }

    std::optional<T> wait_pop() {
        std::unique_lock<std::mutex> lock{mutex_};
        ready_.wait(lock, [this] { return closed_ || size_ != 0u; });
        if (size_ == 0u) return std::nullopt;
        T value = std::move(data_[tail_]);
        tail_ = (tail_ + 1u) % Capacity;
        --size_;
        return value;
    }

    void close() {
        std::lock_guard<std::mutex> lock{mutex_};
        closed_ = true;
        ready_.notify_all();
    }

private:
    std::array<T, Capacity> data_{};
    std::size_t head_{};
    std::size_t tail_{};
    std::size_t size_{};
    bool closed_{};
    std::mutex mutex_;
    std::condition_variable ready_;
};
}
