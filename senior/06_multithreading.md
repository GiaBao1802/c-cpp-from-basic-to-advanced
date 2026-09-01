# Multithreading và C++ memory model

## Race, atomicity, visibility, ordering

Data race trên non-atomic object trong C++ là undefined behavior. Mutex tạo mutual exclusion và happens-before. Atomic cung cấp indivisible operation cùng memory ordering; không tự làm multi-variable invariant atomic.

```cpp
std::mutex m;
std::condition_variable cv;
std::queue<Job> jobs;
bool stopping = false;
```

Consumer phải wait với predicate vì spurious wakeup:

```cpp
cv.wait(lock, [&]{ return stopping || !jobs.empty(); });
```

## Thread lifecycle

Mọi thread cần owner, start barrier, cancellation/shutdown protocol, join và exception/error propagation. Detached thread thường làm lifetime/test khó.

## Deadlock

Bốn điều kiện: mutual exclusion, hold-and-wait, no preemption, circular wait. Phòng bằng global lock order, `std::scoped_lock`, giảm nested lock, message passing và timeout chỉ khi recovery có nghĩa.

## Atomics

`memory_order_seq_cst` dễ reasoning nhất; acquire/release xây publish-consume; relaxed chỉ atomicity/modification order. Chỉ tối ưu ordering sau khi có invariant, benchmark và test; lock-free không đồng nghĩa wait-free hay nhanh hơn.

ABA, false sharing, cache line bouncing, priority inversion và reclamation làm lock-free production khó. Dùng proven queue/library nếu không có lý do mạnh.

## Bounded producer–consumer

ADAS/telemetry pipeline cần bounded queue và overload policy: block, drop newest, drop oldest, coalesce hoặc degrade. Queue unbounded phá latency/memory. Metrics gồm depth high-water, dropped count, wait time và end-to-end latency.

## AUTOSAR OS so với Linux threads

AUTOSAR Classic task/ISR static priority và resource protocol; Linux/POSIX thread có scheduler/policy khác. Concept race/deadlock/ownership giống nhau, API và timing guarantee khác. Không bê `std::thread` vào MCAL.

## Test concurrency

Stress nhiều schedule, ThreadSanitizer, deterministic fake clock, fault/cancellation injection, repeated startup/shutdown và invariant assertions. Một lần chạy đúng không chứng minh thread-safe.
