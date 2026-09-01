#include "ipc_publisher.hpp"

#include <cstdio>
#include <memory>

#ifdef __linux__
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

namespace adas {
#ifdef __linux__
class UnixDatagramPublisher final : public IpcPublisher {
public:
    UnixDatagramPublisher() : fd_{::socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0)} {}
    ~UnixDatagramPublisher() override { if (fd_ >= 0) ::close(fd_); }
    bool publish(const Decision& decision) noexcept override {
        if (fd_ < 0) return false;
        sockaddr_un address{};
        address.sun_family = AF_UNIX;
        (void)std::snprintf(address.sun_path, sizeof(address.sun_path), "%s", "/tmp/adas_monitor.sock");
        const unsigned char message[3] = {1u, decision.risk,
                                          static_cast<unsigned char>(decision.valid ? 1u : 0u)};
        return ::sendto(fd_, message, sizeof(message), MSG_DONTWAIT,
                        reinterpret_cast<const sockaddr *>(&address), sizeof(address)) ==
               static_cast<ssize_t>(sizeof(message));
    }
private:
    int fd_;
};
#else
class NullPublisher final : public IpcPublisher {
public:
    bool publish(const Decision&) noexcept override { return false; }
};
#endif

std::unique_ptr<IpcPublisher> make_local_ipc_publisher()
{
#ifdef __linux__
    return std::make_unique<UnixDatagramPublisher>();
#else
    return std::make_unique<NullPublisher>();
#endif
}
}
