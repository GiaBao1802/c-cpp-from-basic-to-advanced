# Senior C/C++ engineering

## Architecture trước code

Tách control plane/data plane, define component boundary, synchronous/asynchronous API, ownership, threading, error model và deployment. Diagram phải thể hiện data direction và state owner, không chỉ hộp đẹp.

## API design

Contract gồm valid input, output, lifetime, thread-safety, blocking, complexity, error, cancellation và versioning. Boolean return thường thiếu context; status enum/result type rõ hơn. Không expose implementation/container type khi boundary cần ổn định.

## Performance

Đo trước tối ưu. Xác định latency percentile/WCET, throughput, CPU, allocation, cache miss, contention và memory high-water. Algorithm/layout thường quan trọng hơn micro-optimization. Đọc assembly chỉ khi profiler/timing chỉ đúng hotspot.

## Reliability

Timeout phải đi cùng recovery; retry cần backoff/idempotency và budget; circuit breaker/degraded mode cần requirement. Assert dùng cho programmer invariant, không thay runtime validation của external input.

## Security

Validate length trước parse, integer overflow trước allocate/copy, least privilege, secure update/key handling, dependency scanning, compiler hardening và fuzz network/file parser. Memory-safe wrapper giảm rủi ro nhưng C++ vẫn có dangling/race/UB.

## Code review senior

Review requirement/architecture trước style. Hỏi lifetime, range, concurrency, failure path, compatibility, observability và tests. Phân loại comment theo correctness/risk/maintainability; giải thích nguyên nhân và đề xuất có thể kiểm chứng.

## Incident debugging

1. Ổn định/contain impact.
2. Thu thập evidence trước restart nếu an toàn.
3. Xây timeline và last-known-good boundary.
4. Reproduce/minimize; hypothesis có falsification test.
5. Fix root cause, thêm regression/monitor.
6. Blameless retrospective: detection, response, systemic prevention.

## Technical leadership

Breakdown theo deliverable và risk; xác định dependency/critical path; prototype unknown sớm; review interface trước implementation; mentor bằng reasoning; báo blocker kèm impact/options. Estimate phải có assumption và confidence, không chỉ ngày hứa.

## Senior interview system-design prompt

Thiết kế service nhận 4 sensor streams, đồng bộ timestamp, chạy fusion 20 ms, publish result và degrade khi mất một sensor. Trình bày input bound, queue/drop policy, thread/core allocation, ownership, time base, monitoring, restart, test và memory/CPU budget.
