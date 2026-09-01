# Design patterns trong Embedded và ADAS

Pattern là tên cho giải pháp lặp lại, không phải mục tiêu. Áp dụng khi nó làm ownership/dependency/change point rõ hơn.

## Strategy

Thay algorithm theo configuration/mode: checksum profile, filtering, actuator policy. C dùng function pointer table; C++ dùng interface/template. Nếu chỉ có một behavior, abstraction có thể thừa.

## State

Diagnostic session, network manager, ECU lifecycle. State object hoặc table làm transition/action rõ. Luôn định nghĩa invalid event, timeout, entry/exit và recovery.

## Observer/Event bus

Một event nhiều subscriber. Rủi ro: callback order, lifetime, reentrancy, hidden timing. AUTOSAR RTE/event mechanism thường tốt hơn observer tự chế trong application.

## Adapter

Bọc vendor/legacy API thành interface domain. Hữu ích khi test algorithm không phụ thuộc MCAL/Linux socket. Adapter không nên che mất semantics asynchronous/ownership.

## Facade

Cung cấp API đơn giản cho subsystem phức tạp, ví dụ diagnostic client. Tránh “god facade” chứa mọi responsibility.

## Factory/Abstract factory

Chọn implementation theo platform/variant. Embedded static configuration có thể dùng compile-time factory/object table thay heap factory.

## Command

Đóng gói request và data để queue/retry/audit. Dùng cho diagnostic routine/job pipeline. Phải bound queue và define cancellation/idempotency.

## Reactor và Active Object

Reactor dispatch readiness trên event loop (`epoll`); Active Object có thread + queue riêng. Chúng giải coupling nhưng thêm queue latency/shutdown complexity.

## Object pool

Preallocate frame/job object, deterministic acquire/release. Cần exhaustion policy, generation counter chống stale handle và double-free detection.

## Patterns cần thận trọng

Singleton là global state trá hình, khó test/lifecycle/multicore. Service Locator giấu dependency. Deep inheritance làm behavior khó trace. Senior thường ưu tiên explicit dependency, composition và generated/static configuration.

## Pattern review template

Vấn đề cụ thể? Change point? Owner/lifetime? Runtime/memory overhead? Failure behavior? Testability? Alternative đơn giản? Nếu không trả lời được, chưa nên thêm pattern.
