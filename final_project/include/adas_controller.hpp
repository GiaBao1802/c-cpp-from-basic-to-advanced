#pragma once

#include "adas_types.hpp"
#include "bounded_queue.hpp"
#include "fusion.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

namespace adas {
class DecisionObserver {
public:
    virtual ~DecisionObserver() = default;
    virtual void on_decision(const Decision& decision) = 0;
};

class AdasController final {
public:
    AdasController(std::unique_ptr<FusionStrategy> fusion,
                   std::shared_ptr<DecisionObserver> observer);
    ~AdasController();
    AdasController(const AdasController&) = delete;
    AdasController& operator=(const AdasController&) = delete;

    void start();
    void stop();
    bool submit(Detection detection);
    Decision last_decision() const;
    std::uint32_t dropped() const noexcept { return dropped_.load(); }

private:
    void run();
    BoundedQueue<Detection, 8> input_;
    std::unique_ptr<FusionStrategy> fusion_;
    std::weak_ptr<DecisionObserver> observer_;
    std::thread worker_;
    mutable std::mutex state_mutex_;
    Decision last_{};
    std::optional<Detection> radar_;
    std::optional<Detection> camera_;
    std::atomic<std::uint32_t> dropped_{};
    std::atomic<bool> running_{};
};
}
