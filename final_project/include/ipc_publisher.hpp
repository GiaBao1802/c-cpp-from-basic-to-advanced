#pragma once

#include "adas_types.hpp"

#include <memory>

namespace adas {
class IpcPublisher {
public:
    virtual ~IpcPublisher() = default;
    virtual bool publish(const Decision& decision) noexcept = 0;
};

std::unique_ptr<IpcPublisher> make_local_ipc_publisher();
}
