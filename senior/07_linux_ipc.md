# C/C++ trên Linux và IPC

## Process, thread và virtual memory

Process có virtual address space và resources riêng; thread trong process chia sẻ address space/file descriptors nhưng có stack riêng. `fork()` tạo process image copy-on-write; `exec()` thay program image. Luôn xử lý return value, `errno`, partial operation và interruption (`EINTR`).

## File descriptor và I/O

File, socket, pipe, device đều được thao tác qua descriptor. `read/write` có thể trả ít byte hơn yêu cầu; TCP là byte stream, không giữ message boundary. Non-blocking I/O trả `EAGAIN`; event loop dùng `poll/epoll`.

```mermaid
flowchart LR
  NIC["socket/device"] --> K["kernel readiness"] --> EP["epoll"] --> LOOP["event loop"] --> WORK["bounded workers"]
```

Edge-triggered epoll phải drain tới `EAGAIN`. Level-triggered dễ reasoning hơn nhưng có thể wake lặp. Không block event-loop bằng compute dài.

## IPC comparison

| IPC | Ưu điểm | Rủi ro/use case |
|---|---|---|
| pipe/FIFO | đơn giản stream | local, framing/backpressure |
| Unix domain socket | bidirectional, credentials | local services |
| TCP/UDP | network | serialization, partial/loss/order |
| shared memory | throughput/zero-copy | synchronization, crash consistency |
| message queue | message boundary/priority | limits, cleanup |
| signal | notification | ít payload, async-signal-safe |

Shared memory chỉ chia bytes; vẫn cần process-shared mutex/semaphore/futex hoặc lock-free protocol, versioned layout và owner-death recovery.

## RAII wrapper

Linux descriptor nên được bọc movable/non-copyable; destructor `close()`. Nhưng destructor không thể báo lỗi tốt, nên operation quan trọng như flush/commit cần explicit API.

## Daemon production checklist

Config validation, least privilege, signal-safe shutdown notification, graceful drain, watchdog/health endpoint, structured logs, metrics, log rotation, resource limits, reconnect backoff, clock choice (`CLOCK_MONOTONIC` cho duration), crash dump và systemd restart policy.

## Build/debug tools

GDB, core dump, `strace`, `ltrace`, `perf`, sanitizers (ASan/UBSan/TSan), Valgrind khi phù hợp, `nm/readelf/objdump`, `/proc`, flame graph. Debug optimized build cần hiểu inlining/omitted variables.

## Automotive Linux/ADAS

Service có thể nhận sensor/network data, IPC với perception/control, publish telemetry. Safety boundary, time synchronization, bounded resource, startup dependency và degraded mode phải được thiết kế; Linux không mặc nhiên real-time. PREEMPT_RT/scheduler affinity giúp nhưng vẫn cần measurement.
